#include "Application.hpp"

#include <iostream>
#include <cmath> 
#include <glmlv/Image2DRGBA.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>

#ifndef TINYGLTF_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>
#endif

int Application::run()
{
	float clearColor[3] = { 0.5, 0.1, 0.1 };
	glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);


	bool directionalSMResolutionDirty = false;
	bool directionalSMDirty = true;


    // Loop until the user closes the windows
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        // WORLD MATRIX
        if (m_model.cameras.size() > 0)
        {
            tinygltf::Camera cam = m_model.cameras[0];
            if (cam.type == "perspective")
            {
                m_projMatrix = glm::perspective(cam.perspective.yfov, cam.perspective.aspectRatio, cam.perspective.znear, cam.perspective.zfar);  
            }
            else if (cam.type == "orthographic")
            {
                m_projMatrix = glm::ortho(-cam.orthographic.xmag, cam.orthographic.xmag, -cam.orthographic.ymag, cam.orthographic.ymag, cam.orthographic.znear, cam.orthographic.zfar);
            }
        }
        else
        {
            m_projMatrix = glm::perspective(70.f, float(m_nWindowWidth) / m_nWindowHeight, 0.01f, 100.f);            
        }
        
        m_viewMatrix = m_viewController.getViewMatrix();
		const auto rcpViewMatrix = m_viewController.getRcpViewMatrix();

		float m_SceneSizeLength = 100.f;
		const float sceneRadius = m_SceneSizeLength * 0.5f;

		const auto dirLightUpVector = computeDirectionVectorUp(glm::radians(m_DirLightPhiAngleDegrees), glm::radians(m_DirLightThetaAngleDegrees));
		const auto dirLightViewMatrix = glm::lookAt(center + m_DirLightDirection * sceneRadius, center, dirLightUpVector); // Will not work if m_DirLightDirection is colinear to lightUpVector
		const auto dirLightProjMatrix = glm::ortho(-sceneRadius, sceneRadius, -sceneRadius, sceneRadius, 0.01f * sceneRadius, 2.f * sceneRadius);



		// ==== SHADOW MAP ==== //

		// Shadow map computation if necessary
		{
			if (directionalSMResolutionDirty)
			{
				glDeleteTextures(1, &m_directionalSMTexture);

				// Realocate texture
				glGenTextures(1, &m_directionalSMTexture);
				glBindTexture(GL_TEXTURE_2D, m_directionalSMTexture);
				glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, m_nDirectionalSMResolution, m_nDirectionalSMResolution);
				glBindTexture(GL_TEXTURE_2D, 0);

				// Attach new texture to FBO
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_directionalSMFBO);
				glBindTexture(GL_TEXTURE_2D, m_directionalSMTexture);
				glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_directionalSMTexture, 0);

				const auto fboStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
				if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
				{
					std::cerr << "Error on building directional shadow mapping framebuffer. Error code = " << fboStatus << std::endl;
					throw std::runtime_error("Error on building directional shadow mapping framebuffer.");
				}

				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

				directionalSMResolutionDirty = false;
				directionalSMDirty = true; // The shadow map must also be recomputed
			}

			if (directionalSMDirty)
			{
				m_directionalSMProgram.use();

				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_directionalSMFBO);
				glViewport(0, 0, m_nDirectionalSMResolution, m_nDirectionalSMResolution);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glUniformMatrix4fv(m_uDirLightViewProjMatrix, 1, GL_FALSE, glm::value_ptr(dirLightProjMatrix * dirLightViewMatrix));

				//glBindVertexArray(m_TriangleVAO);

				// We draw each shape by specifying how much indices it carries, and with an offset in the global index buffer
				//for (const auto shape : m_shapes) {
					//glDrawElements(GL_TRIANGLES, shape.indexCount, GL_UNSIGNED_INT, (const GLvoid*)(shape.indexOffset * sizeof(GLuint)));
				//}

				DrawModel(m_model);


				glBindVertexArray(0);

				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

				directionalSMDirty = false;
			}
		}
        // ==== GEOMETRY PASS ==== //
        {
            m_geometryPassProgram.use();
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_GBufferFBO);

            glViewport(0, 0, m_nWindowWidth, m_nWindowHeight);
			
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            for (GLuint i : {0, 1})
                glBindSampler(i, m_textureSampler);

            // Set texture unit of each sampler
            glUniform1i(m_uKaSamplerLocation, 0);
            glUniform1i(m_uKdSamplerLocation, 1);

            // GLTF DRAWING
            DrawModel(m_model);

            // UNBIND
            for (GLuint i : {0, 1})
                glBindSampler(0, m_textureSampler);                
            
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        }

        const auto viewportSize = m_GLFWHandle.framebufferSize();
        glViewport(0, 0, viewportSize.x, viewportSize.y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ==== SHADING PASS ==== //
        if (m_CurrentlyDisplayed == GBufferTextureCount) // BEAUTY
        {            
            {
                m_shadingPassProgram.use();

                glUniform3fv(m_uDirectionalLightDirLocation, 1, glm::value_ptr(glm::vec3(m_viewMatrix * glm::vec4(glm::normalize(m_DirLightDirection), 0))));
                glUniform3fv(m_uDirectionalLightIntensityLocation, 1, glm::value_ptr(m_DirLightColor * m_DirLightIntensity));

                glUniform3fv(m_uPointLightPositionLocation, 1, glm::value_ptr(glm::vec3(m_viewMatrix * glm::vec4(m_PointLightPosition, 1))));
                glUniform3fv(m_uPointLightIntensityLocation, 1, glm::value_ptr(m_PointLightColor * m_PointLightIntensity));

				glUniform1fv(m_uDirLightShadowMapBias, 1, &m_DirLightSMBias);

				glUniform1iv(m_uDirLightShadowMapSampleCount, 1, &m_DirLightSMSampleCount);
				glUniform1fv(m_uDirLightShadowMapSpread, 1, &m_DirLightSMSpread);

				glUniformMatrix4fv(m_uDirLightViewProjMatrix_shadingPass, 1, GL_FALSE, glm::value_ptr(dirLightProjMatrix * dirLightViewMatrix * rcpViewMatrix));


                for (int32_t i = GPosition; i < GDepth; ++i)
                {
                    glActiveTexture(GL_TEXTURE0 + i);
                    glBindTexture(GL_TEXTURE_2D, m_GBufferTextures[i]);

                    glUniform1i(m_uGBufferSamplerLocations[i], i);
                }

				glActiveTexture(GL_TEXTURE0 + GDepth);
				glBindTexture(GL_TEXTURE_2D, m_directionalSMTexture);
				glBindSampler(GDepth, m_directionalSMSampler);
				glUniform1i(m_uDirLightShadowMap, GDepth);

                glBindVertexArray(m_TriangleVAO);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                glBindVertexArray(0);
            }
        }
        else if (m_CurrentlyDisplayed == GDepth)    // DEPTH
        {
            m_displayDepthProgram.use();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_GBufferTextures[GDepth]);

            glUniform1i(m_uGDepthSamplerLocation, 0);

            glBindVertexArray(m_TriangleVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
        }
        else if (m_CurrentlyDisplayed == GPosition) // POSITION
        {
            m_displayPositionProgram.use();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_GBufferTextures[GPosition]);

            glUniform1i(m_uGDepthSamplerLocation, 0);

            const auto rcpProjMat = glm::inverse(m_projMatrix);

            const glm::vec4 frustrumTopRight(1, 1, 1, 1);
            const auto frustrumTopRight_view = rcpProjMat * frustrumTopRight;

            glUniform3fv(m_uSceneSizeLocation, 1, glm::value_ptr(frustrumTopRight_view / frustrumTopRight_view.w));

            glBindVertexArray(m_TriangleVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
        }
		else if (m_CurrentlyDisplayed == Display_DirectionalLightDepthMap)
		{
			m_displayDepthProgram.use();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_directionalSMTexture);

			glUniform1i(m_uGDepthSamplerLocation, 0);

			glBindVertexArray(m_TriangleVAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glBindVertexArray(0);
		}
        else    // NORMAL / AMBIENT / GS
        {
            // GBuffer display
            glBindFramebuffer(GL_READ_FRAMEBUFFER, m_GBufferFBO);
			glReadBuffer(GL_COLOR_ATTACHMENT0 + m_CurrentlyDisplayed);
			glBlitFramebuffer(0, 0, m_nWindowWidth, m_nWindowHeight,
				0, 0, m_nWindowWidth, m_nWindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        }
        


        // GUI code:
		glmlv::imguiNewFrame();

        {
            ImGui::Begin("GUI");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            if (ImGui::ColorEdit3("clearColor", clearColor)) {
                glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);
            }
            if (ImGui::CollapsingHeader("Directional Light"))
            {
				ImGui::ColorEdit3("DirLightColor", glm::value_ptr(m_DirLightColor));
				ImGui::DragFloat("DirLightIntensity", &m_DirLightIntensity, 0.1f, 0.f, 100.f);
				bool angleChanged = ImGui::DragFloat("Phi Angle", &m_DirLightPhiAngleDegrees, 1.0f, 0.0f, 360.f);
				angleChanged = ImGui::DragFloat("Theta Angle", &m_DirLightThetaAngleDegrees, 1.0f, 0.0f, 180.f) || angleChanged;

				if (angleChanged) {
					m_DirLightDirection = computeDirectionVector(glm::radians(m_DirLightPhiAngleDegrees), glm::radians(m_DirLightThetaAngleDegrees));
					directionalSMDirty = true;
				}

				if (ImGui::InputInt("DirShadowMap Res", &m_nDirectionalSMResolution))
				{
					if (m_nDirectionalSMResolution <= 0)
						m_nDirectionalSMResolution = 1;
					directionalSMResolutionDirty = true;
				}

				ImGui::InputFloat("DirShadowMap Bias", &m_DirLightSMBias);
				ImGui::InputInt("DirShadowMap SampleCount", &m_DirLightSMSampleCount);
				ImGui::InputFloat("DirShadowMap Spread", &m_DirLightSMSpread);
            }

            if (ImGui::CollapsingHeader("GBuffer"))
            {
                for (int32_t i = GPosition; i <= GBufferTextureCount; ++i)
                {
                    if (ImGui::RadioButton(m_GBufferTexNames[i], m_CurrentlyDisplayed == i))
                        m_CurrentlyDisplayed = GBufferTextureType(i);
                }
            }

            ImGui::End();
        }

		glmlv::imguiRenderFrame();

        /* Poll for and process events */
        glfwPollEvents();

        /* Swap front and back buffers*/
        m_GLFWHandle.swapBuffers();

        auto ellapsedTime = glfwGetTime() - seconds;
        auto guiHasFocus = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
        if (!guiHasFocus) {
            m_viewController.update(float(ellapsedTime));
        }
    }

    return 0;
}

Application::Application(int argc, char** argv):
    m_AppPath { glmlv::fs::path{ argv[0] } },
    m_AppName { m_AppPath.stem().string() },
    m_ImGuiIniFilename { m_AppName + ".imgui.ini" },
    m_ShadersRootPath { m_AppPath.parent_path() / "shaders" },
    m_AssetsRootPath { m_AppPath.parent_path() / "assets" }

{
    ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file

    // 0 - LINK WITH SHADERS

    // Attribute location
    const GLint positionAttrLocation = 0;
    const GLint normalAttrLocation = 1;
    const GLint texCoordsAttrLocation = 2;
    
    // Map gltf attributes name to an index value of vs
    m_attribs["POSITION"] = positionAttrLocation;
    m_attribs["NORMAL"] = normalAttrLocation;
    m_attribs["TEXCOORD_0"] = texCoordsAttrLocation;

    // 1 - LOAD SCENE (.GLTF)
    if (argc < 2) {
        printf("Needs input.gltf\n");
        exit(1);
    }
    const glmlv::fs::path gltfPath = m_AssetsRootPath / m_AppName / glmlv::fs::path{ argv[1] };

    loadTinyGLTF(gltfPath);

    // 2 - CREATE SHADER PROGRAMS
    initShadersData();

    glEnable(GL_DEPTH_TEST);

    // Init GBuffer
    glGenTextures(GBufferTextureCount, m_GBufferTextures);

    for (int32_t i = GPosition; i < GBufferTextureCount; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, m_GBufferTextures[i]);
        glTexStorage2D(GL_TEXTURE_2D, 1, m_GBufferTextureFormat[i], m_nWindowWidth, m_nWindowHeight);
    }

    glGenFramebuffers(1, &m_GBufferFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_GBufferFBO);
    for (int32_t i = GPosition; i < GDepth; ++i)
    {
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_GBufferTextures[i], 0);
    }
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_GBufferTextures[GDepth], 0);

    // we will write into 5 textures from the fragment shader
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
    glDrawBuffers(5, drawBuffers);

    GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "FB error, status: " << status << std::endl;
        throw std::runtime_error("FBO error");
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    // TRIANGLE COVERING THE SCREEN FOR SHADING PASS
    glGenBuffers(1, &m_TriangleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_TriangleVBO);

    GLfloat data[] = { -1, -1, 3, -1, -1, 3 };
    glBufferStorage(GL_ARRAY_BUFFER, sizeof(data), data, 0);

    glGenVertexArrays(1, &m_TriangleVAO);
    glBindVertexArray(m_TriangleVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // 3 - SET CAMERA POSTION
    //m_viewController.setViewMatrix(glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));


    glm::vec3 center1 = GetCenterOfModel();
    std::cout << "Center Method 1 : " << center1 << std::endl;
    
    BoundingBox boundingBox = CreateBoundingBox();
    glm::vec3 center2 = GetCenterOfBoundingBox(boundingBox);
    std::cout << "Center Method 2 : " << center2 << std::endl;

    if (argc == 3)
    {
        if(!strcmp(argv[2], "c1"))
        {
            center = center1;
        }
        else if (!strcmp(argv[2], "c2"))
        {
            center = center2;
        }
    }
    glm::vec3 modelDimension = boundingBox.max - boundingBox.min;
    std::cout << "Model dimensions : " << modelDimension << std::endl;

    // TODO --> Find a real formula in order to have a nice zDistance to look at the model at a right distance
    // If the model has a width of 2, then we will be -5 distance behind center
    // Formula found just by testing some values
    //float zDistance = 4.5 + 4 * ((modelDimension.x >= modelDimension.y) ? modelDimension.x/15 : modelDimension.y/15);
    //float zDistance = 5.0f;    
	
	float vfov = 182 * (3.14 / 180);
	float zDistance = m_nWindowHeight / (2 * std::tan(vfov / 2));
	
    std::cout << "zDistance : " << zDistance << std::endl;

    // TODO --> Find the real Vector Up of the model and the real Vector Forward
    m_viewController.setViewMatrix(glm::lookAt(glm::vec3(center.x, center.y, center.z + zDistance), center, glm::vec3(0, 1, 0)));
    m_viewController.setSpeed(modelDimension.x * 0.5f); // Let's travel 10% of the scene per second
}

// ------ GLTF INITIALIZATION --------

// Load an obj m_model with tinyGLTF
void Application::loadTinyGLTF(const glmlv::fs::path & gltfPath)
{
    // 1 - LOAD
    //tinygltf::Model m_model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool ret = false;
    std::cout << "Reading ASCII glTF" << std::endl;
    // assume ascii glTF.
	std::cout << gltfPath.string() << std::endl;
    ret = loader.LoadASCIIFromFile(&m_model, &err, &warn, gltfPath.string());

    // Catch errors
    if (!warn.empty()) {
        printf("Warn: %s\n", warn.c_str());
    }

    if (!err.empty()) {
        printf("Err: %s\n", err.c_str());
    }

    if (!ret) {
        printf("Failed to parse glTF\n");
    }


    // 2 - BUFFERS (VBO / IBO)
    
    std::vector<GLuint> buffers(m_model.buffers.size()); // un par tinygltf::Buffer
    
    glGenBuffers(buffers.size(), buffers.data());
    for (size_t i = 0; i < buffers.size(); ++i)
    {
        const tinygltf::BufferView &bufferView = m_model.bufferViews[i];
        glBindBuffer(bufferView.target, buffers[i]);
        glBufferStorage(bufferView.target, m_model.buffers[i].data.size(), m_model.buffers[i].data.data(), 0);
        glBindBuffer(bufferView.target, 0);
    }

    std::cout << "# of buffers : " << buffers.size() << std::endl;
    std::cout << "# of meshes : " << m_model.meshes.size() << std::endl;
    
    // 3 - VAOs & Primitives
    // Pour chaque VAO on va aussi stocker les données de la primitive associé car on doit l'utiliser lors du rendu

    // Pour chaque mesh
    for (size_t i = 0; i < m_model.meshes.size(); i++)
    {
        // Store the mesh infos
        MeshInfos meshInfos;

        const tinygltf::Mesh &mesh = m_model.meshes[i];
        std::cout << "# of primitives : " << mesh.primitives.size() << std::endl;

        // Pour chaque primitives du mesh
        for (size_t primId = 0; primId < mesh.primitives.size(); primId++)				
        {
            const tinygltf::Primitive &primitive = mesh.primitives[primId];

            // 3.1 - CREATE VAO FOR EACH PRIMITIVE
            // Generate VAO
            GLuint vaoId;
            glGenVertexArrays(1, &vaoId);
            glBindVertexArray(vaoId);

            // 3.2 - INDICES (IBO)
            tinygltf::Accessor &indexAccessor = m_model.accessors[primitive.indices];
            tinygltf::BufferView &bufferView = m_model.bufferViews[indexAccessor.bufferView];
            int &bufferIndex = bufferView.buffer;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[bufferIndex]); // Binding IBO
            
            // 3.3 - ATTRIBUTS
            // Pour chaque attributes de la primitive
            std::map<std::string, int>::const_iterator it(primitive.attributes.begin());
            std::map<std::string, int>::const_iterator itEnd(primitive.attributes.end());

            for (; it != itEnd; it++)
            {
                assert(it->second >= 0);
                tinygltf::Accessor &accessor = m_model.accessors[it->second];
                bufferView = m_model.bufferViews[accessor.bufferView];
                bufferIndex = bufferView.buffer;
                glBindBuffer(GL_ARRAY_BUFFER, buffers[bufferIndex]);    // Binding VBO
                
                // Get number of component
                int size = 1;
                if (accessor.type == TINYGLTF_TYPE_SCALAR) {
                    size = 1;
                } else if (accessor.type == TINYGLTF_TYPE_VEC2) {
                    size = 2;
                } else if (accessor.type == TINYGLTF_TYPE_VEC3) {
                    size = 3;
                } else if (accessor.type == TINYGLTF_TYPE_VEC4) {
                    size = 4;
                } else {
                    assert(0);
                }                
                
                // it->first would be "POSITION", "NORMAL", "TEXCOORD_0", ...
                if (m_attribs.count(it->first) > 0)
                {
                    // Compute byteStride from Accessor + BufferView combination.
                    int byteStride =  accessor.ByteStride(m_model.bufferViews[accessor.bufferView]);
                    assert(byteStride != -1);
                    glEnableVertexAttribArray(m_attribs[it->first]);
                    glVertexAttribPointer(m_attribs[it->first], size /*accessor.type*/, accessor.componentType, accessor.normalized ? GL_TRUE : GL_FALSE, byteStride, (const GLvoid*) (bufferView.byteOffset + accessor.byteOffset));

                    // If it's the position attribute
                    if (it->first.compare("POSITION") == 0)
                    {
                        // 3.3 BIS - CENTER FOR CAMERA
                        // For Method 1
                        meshInfos.centers.push_back(GetCenterOfPrimitive(accessor.minValues, accessor.maxValues));
                        // For Method 2
                        meshInfos.min.push_back(glm::make_vec3(accessor.minValues.data()));
                        meshInfos.max.push_back(glm::make_vec3(accessor.maxValues.data()));
                    }                        
                }

                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }

            // On rempli le vao et les primitives
            meshInfos.vaos.push_back(vaoId);
            meshInfos.primitives.push_back(primitive);

            glBindVertexArray(0);

            // 3.4 - TEXTURES
            meshInfos.diffuseColor.push_back(glm::vec4(1));
            meshInfos.diffuseTexture.push_back(0);
            meshInfos.emissiveColor.push_back(glm::vec4(0));
            meshInfos.emissiveTexture.push_back(0);           

            if (primitive.material < 0) {
					continue;
            }

            tinygltf::Material &mat = m_model.materials[primitive.material];   

            // Diffuse Color Factor
            if (mat.values.find("baseColorFactor") != mat.values.end())
            {
                const auto& parameter = mat.values["baseColorFactor"];

                tinygltf::ColorValue color = parameter.ColorFactor();

                meshInfos.diffuseColor.back() = glm::make_vec4(color.data());
            }         

            // Diffuse Color Texture
            if (mat.values.find("baseColorTexture") != mat.values.end())
            {
                const auto& parameter = mat.values["baseColorTexture"];

                tinygltf::Texture &tex = m_model.textures[parameter.TextureIndex()];

                AddTexture(tex, meshInfos, true, false);
            }
            // Emissive Color
            if (mat.additionalValues.find("emissiveFactor") != mat.additionalValues.end())
            {
                const auto& parameter = mat.additionalValues["emissiveFactor"];

                tinygltf::ColorValue color = parameter.ColorFactor();

                meshInfos.emissiveColor.back() = glm::make_vec3(color.data());
            }  
            // Emissive Texture
            if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end())
            {
                const auto& parameter = mat.additionalValues["emissiveTexture"];

                tinygltf::Texture &tex = m_model.textures[parameter.TextureIndex()];

                // TODO --> Find why the model is still black (we find the right image though)
                AddTexture(tex, meshInfos, false, true);
            }
        }

        // VSCode signal an error here but there's no error
        m_meshInfos.push_back(meshInfos);
    }

    // SAMPLER --> TODO --> Handle multiple samplers
    glGenSamplers(1, &m_textureSampler);
    if (m_model.samplers.size() > 0)
    {
        tinygltf::Sampler sampler = m_model.samplers[0];
        glSamplerParameteri(m_textureSampler, GL_TEXTURE_MIN_FILTER, sampler.minFilter);
        glSamplerParameteri(m_textureSampler, GL_TEXTURE_MAG_FILTER, sampler.magFilter);
        glSamplerParameteri(m_textureSampler, GL_TEXTURE_WRAP_S, sampler.wrapS);
        glSamplerParameteri(m_textureSampler, GL_TEXTURE_WRAP_T, sampler.wrapT);
        glSamplerParameteri(m_textureSampler, GL_TEXTURE_WRAP_R, sampler.wrapR);
    }
    else
    {
        glSamplerParameteri(m_textureSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(m_textureSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
}

GLenum Application::getMode(int mode)
{
    if (mode == TINYGLTF_MODE_TRIANGLES) {
        return GL_TRIANGLES;
    } else if (mode == TINYGLTF_MODE_TRIANGLE_STRIP) {
        return GL_TRIANGLE_STRIP;
    } else if (mode == TINYGLTF_MODE_TRIANGLE_FAN) {
        return GL_TRIANGLE_FAN;
    } else if (mode == TINYGLTF_MODE_POINTS) {
        return GL_POINTS;
    } else if (mode == TINYGLTF_MODE_LINE) {
        return GL_LINES;
    } else if (mode == TINYGLTF_MODE_LINE_LOOP) {
        return GL_LINE_LOOP;
    } else {
        assert(0);
    }
}

void Application::AddTexture(tinygltf::Texture &tex, MeshInfos& meshInfos, bool diffuse, bool emissive)
{
    if (tex.source > -1 && tex.source < m_model.images.size())
    {
        tinygltf::Image &image = m_model.images[tex.source];

        if (emissive)
        {
            glActiveTexture(GL_TEXTURE0);
        }
        else if (diffuse)
        {
            glActiveTexture(GL_TEXTURE1);
        }
        

        GLuint texId;
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);

        // Ignore Texture.fomat.
        GLenum format = GL_RGBA;
        if (image.component == 3) {
            format = GL_RGB;
        }
        
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        

        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, image.width, image.height);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width, image.height, GL_RGBA, GL_UNSIGNED_BYTE, &image.image.at(0));
        
        glBindTexture(GL_TEXTURE_2D, 0);
        
        if (emissive)
        {
            meshInfos.emissiveTexture.back() = texId;
        }
        else if (diffuse)
        {
            meshInfos.diffuseTexture.back() = texId;
        }    
    }
}

// ------ GLTF DRAW --------


void Application::DrawModel(tinygltf::Model &model) {
  // If the glTF asset has at least one scene, and doesn't define a default one
  // just show the first one we can find
  assert(model.scenes.size() > 0);
  int scene_to_display = model.defaultScene > -1 ? model.defaultScene : 0;
  const tinygltf::Scene &scene = model.scenes[scene_to_display];
  
  if (scene.nodes.size() != 0) {
	  for (size_t i = 0; i < scene.nodes.size(); i++)
	  {
		  DrawNode(model, model.nodes[scene.nodes[i]], glm::mat4(1));
	  }
  }

}

// Hierarchically draw nodes
void Application::DrawNode(tinygltf::Model &model, const tinygltf::Node &node, glm::mat4 currentMatrix) {

    // PUSH MATRIX
    glm::mat4 modelMatrix = currentMatrix;

    // Use `matrix' attribute
    if (node.matrix.size() == 16)
    {
        modelMatrix = glm::make_mat4(node.matrix.data());
    }
    // Use `translate', 'rotate' and 'scale' attributes
    // L = T * R * S --> First Scale, then Rotate, then Translate
    else
    {       
        if (node.scale.size() == 3)
        {
            glm::vec3 scale = glm::make_vec3(node.scale.data());
            modelMatrix = glm::scale(modelMatrix, scale);
        }          

        if (node.rotation.size() == 4)
        {
            modelMatrix = quatToMatrix(glm::make_vec4(node.rotation.data())) * modelMatrix;
        }
          
        if (node.translation.size() == 3)
        {
            glm::vec3 translate = glm::make_vec3(node.translation.data());
            modelMatrix = glm::translate(modelMatrix, translate);
        }   
    }

    modelMatrix = modelMatrix * currentMatrix;
    
    if (node.mesh > -1)
    {
        assert(node.mesh < model.meshes.size());
        DrawMesh(node.mesh, modelMatrix);
    }

    // Draw child nodes.
    for (size_t i = 0; i < node.children.size(); i++)
    {
        assert(node.children[i] < model.nodes.size());
        DrawNode(model, model.nodes[node.children[i]], modelMatrix);
    }

    // POP MATRIX
}

void Application::DrawMesh(int meshIndex, glm::mat4 modelMatrix)
{
    // OBJECT MATRIX
    const auto mvMatrix = m_viewMatrix * modelMatrix;
    const auto mvpMatrix = m_projMatrix * mvMatrix;
    const auto normalMatrix = glm::transpose(glm::inverse(mvMatrix));

    glUniformMatrix4fv(m_uModelViewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
    glUniformMatrix4fv(m_uModelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
    glUniformMatrix4fv(m_uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // DRAW EACH VAO / PRIMITIVES of this mesh
    for (size_t i = 0; i < m_meshInfos[meshIndex].vaos.size(); ++i)
    {        
        // Emissive
        glUniform3fv(m_uKaLocation, 1, glm::value_ptr(m_meshInfos[meshIndex].emissiveColor[i]));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_meshInfos[meshIndex].emissiveTexture[i]);
        
        // Diffuse
        glm::vec3 diffuseColor(m_meshInfos[meshIndex].diffuseColor[i].x, m_meshInfos[meshIndex].diffuseColor[i].y, m_meshInfos[meshIndex].diffuseColor[i].z);
        glUniform3fv(m_uKdLocation, 1, glm::value_ptr(diffuseColor));        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_meshInfos[meshIndex].diffuseTexture[i]);

        // Bind VAO
        const tinygltf::Accessor &indexAccessor = m_model.accessors[m_meshInfos[meshIndex].primitives[i].indices];        
        glBindVertexArray(m_meshInfos[meshIndex].vaos[i]);

        // Draw
        glDrawElements(getMode(m_meshInfos[meshIndex].primitives[i].mode), indexAccessor.count, indexAccessor.componentType, (const GLvoid*) indexAccessor.byteOffset);
        
        // Unbind
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

glm::mat4 Application::quatToMatrix(glm::vec4 quaternion)
{
      float sqw = quaternion.w * quaternion.w;
      float sqx = quaternion.x * quaternion.x;
      float sqy = quaternion.y * quaternion.y;
      float sqz = quaternion.z * quaternion.z;

      // invs (inverse square length) is only required if quaternion is not already normalised
      float invs = 1 / (sqx + sqy + sqz + sqw);
      float m00 = ( sqx - sqy - sqz + sqw)*invs ; // since sqw + sqx + sqy + sqz =1/invs*invs
      float m11 = (-sqx + sqy - sqz + sqw)*invs ;
      float m22 = (-sqx - sqy + sqz + sqw)*invs ;

      float tmp1 = quaternion.x*quaternion.y;
      float tmp2 = quaternion.z*quaternion.w;
      float m10 = 2.0 * (tmp1 + tmp2)*invs ;
      float m01 = 2.0 * (tmp1 - tmp2)*invs ;

      tmp1 = quaternion.x*quaternion.z;
      tmp2 = quaternion.y*quaternion.w;
      float m20 = 2.0 * (tmp1 - tmp2)*invs ;
      float m02 = 2.0 * (tmp1 + tmp2)*invs ;
      tmp1 = quaternion.y*quaternion.z;
      tmp2 = quaternion.x*quaternion.w;
      float m21 = 2.0 * (tmp1 + tmp2)*invs ;
      float m12 = 2.0 * (tmp1 - tmp2)*invs ;

      return glm::mat4( m00, m01, m02, 0,
                        m10, m11, m12, 0,
                        m20, m21, m22, 0,
                        0,   0,   0,   1 );
}

// CAMERA CENTER

// METHOD 1
glm::vec3 Application::GetCenterOfPrimitive(const std::vector<double>& min, const std::vector<double>& max)
{
    return glm::vec3(GetMiddle(min[0], max[0]), GetMiddle(min[1], max[1]), GetMiddle(min[2], max[2]));
}

glm::vec3 Application::GetCenterOfMesh(int meshIndex)
{
    return GetMiddle(m_meshInfos[meshIndex].centers);
}

glm::vec3 Application::GetCenterOfModel()
{
    std::vector<glm::vec3> centers;

    for (int i = 0; i < m_meshInfos.size(); ++i)
    {
        centers.push_back(GetCenterOfMesh(i));
    }

    return GetMiddle(centers);
}

// METHOD 2

Application::BoundingBox Application::CreateBoundingBox()
{
    BoundingBox boundingBox;
    boundingBox.Init();

    for (auto meshInfos : m_meshInfos)
    {
        for (int i = 0; i < meshInfos.max.size(); ++i)
        {
            // Get the max coordinates of model
            if (meshInfos.max[i].x > boundingBox.max.x)
            {
                boundingBox.max.x = meshInfos.max[i].x;
            }
            if (meshInfos.max[i].y > boundingBox.max.y)
            {
                boundingBox.max.y = meshInfos.max[i].y;
            }
            if (meshInfos.max[i].z > boundingBox.max.z)
            {
                boundingBox.max.z = meshInfos.max[i].z;
            }

            // Get the min coordinates of model
            if (meshInfos.min[i].x < boundingBox.min.x)
            {
                boundingBox.min.x = meshInfos.min[i].x;
            }
            if (meshInfos.min[i].y < boundingBox.min.y)
            {
                boundingBox.min.y = meshInfos.min[i].y;
            }
            if (meshInfos.min[i].z < boundingBox.min.z)
            {
                boundingBox.min.z = meshInfos.min[i].z;
            }
        }
    }

    return boundingBox;
}

glm::vec3 Application::GetCenterOfBoundingBox(const BoundingBox& boundingBox)
{
    glm::vec3 center;

    center.x = GetMiddle(boundingBox.min.x, boundingBox.max.x);
    center.y = GetMiddle(boundingBox.min.y, boundingBox.max.y);
    center.z = GetMiddle(boundingBox.min.z, boundingBox.max.z);

    return center;
}

//SHADOW MAP
void Application::initShadowData()
{
	glGenTextures(1, &m_directionalSMTexture);

	glBindTexture(GL_TEXTURE_2D, m_directionalSMTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, m_nDirectionalSMResolution, m_nDirectionalSMResolution);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &m_directionalSMFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_directionalSMFBO);
	glBindTexture(GL_TEXTURE_2D, m_directionalSMTexture);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_directionalSMTexture, 0);

	const auto fboStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Error on building directional shadow mapping framebuffer. Error code = " << fboStatus << std::endl;
		throw std::runtime_error("Error on building directional shadow mapping framebuffer.");
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glGenSamplers(1, &m_directionalSMSampler);
	glSamplerParameteri(m_directionalSMSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(m_directionalSMSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(m_directionalSMSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glSamplerParameteri(m_directionalSMSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glSamplerParameteri(m_directionalSMSampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glSamplerParameteri(m_directionalSMSampler, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
}


// SHADERS
// Récupére les locations des uniformes
void Application::initShadersData()
{
    m_geometryPassProgram = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "geometryPass.vs.glsl", m_ShadersRootPath / m_AppName / "geometryPass.fs.glsl" });

    m_uModelViewProjMatrixLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uModelViewProjMatrix");
    m_uModelViewMatrixLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uModelViewMatrix");
    m_uNormalMatrixLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uNormalMatrix");

    m_uKaLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uKa");
    m_uKdLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uKd");
    m_uKsLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uKs");
    m_uShininessLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uShininess");
    m_uKaSamplerLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uKaSampler");
    m_uKdSamplerLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uKdSampler");
    m_uKsSamplerLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uKsSampler");
    m_uShininessSamplerLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uShininessSampler");

    m_shadingPassProgram = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "shadingPass.vs.glsl", m_ShadersRootPath / m_AppName / "shadingPass.fs.glsl" });

    m_uGBufferSamplerLocations[GPosition] = glGetUniformLocation(m_shadingPassProgram.glId(), "uGPosition");
    m_uGBufferSamplerLocations[GNormal] = glGetUniformLocation(m_shadingPassProgram.glId(), "uGNormal");
    m_uGBufferSamplerLocations[GAmbient] = glGetUniformLocation(m_shadingPassProgram.glId(), "uGAmbient");
    m_uGBufferSamplerLocations[GDiffuse] = glGetUniformLocation(m_shadingPassProgram.glId(), "uGDiffuse");
    m_uGBufferSamplerLocations[GGlossyShininess] = glGetUniformLocation(m_shadingPassProgram.glId(), "uGGlossyShininess");

	m_uDirLightViewProjMatrix_shadingPass = glGetUniformLocation(m_shadingPassProgram.glId(), "uDirLightViewProjMatrix");
	m_uDirLightShadowMap = glGetUniformLocation(m_shadingPassProgram.glId(), "uDirLightShadowMap");
	m_uDirLightShadowMapBias = glGetUniformLocation(m_shadingPassProgram.glId(), "uDirLightShadowMapBias");
	m_uDirLightShadowMapSampleCount = glGetUniformLocation(m_shadingPassProgram.glId(), "uDirLightShadowMapSampleCount");
	m_uDirLightShadowMapSpread = glGetUniformLocation(m_shadingPassProgram.glId(), "uDirLightShadowMapSpread");

    
    m_uDirectionalLightDirLocation = glGetUniformLocation(m_shadingPassProgram.glId(), "uDirectionalLightDir");
    m_uDirectionalLightIntensityLocation = glGetUniformLocation(m_shadingPassProgram.glId(), "uDirectionalLightIntensity");

    m_uPointLightPositionLocation = glGetUniformLocation(m_shadingPassProgram.glId(), "uPointLightPosition");
    m_uPointLightIntensityLocation = glGetUniformLocation(m_shadingPassProgram.glId(), "uPointLightIntensity");

    m_displayDepthProgram = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "shadingPass.vs.glsl", m_ShadersRootPath / m_AppName / "displayDepth.fs.glsl" });

    m_uGDepthSamplerLocation = glGetUniformLocation(m_displayDepthProgram.glId(), "uGDepth");

    m_displayPositionProgram = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "shadingPass.vs.glsl", m_ShadersRootPath / m_AppName / "displayPosition.fs.glsl" });

    m_uGPositionSamplerLocation = glGetUniformLocation(m_displayPositionProgram.glId(), "uGPosition");
    m_uSceneSizeLocation = glGetUniformLocation(m_displayPositionProgram.glId(), "uSceneSize");

	m_directionalSMProgram = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "directionalSM.vs.glsl", m_ShadersRootPath / m_AppName / "directionalSM.fs.glsl" });
	m_uDirLightViewProjMatrix = glGetUniformLocation(m_directionalSMProgram.glId(), "uDirLightViewProjMatrix");
}
