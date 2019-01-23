#include "Application.hpp"

#include <iostream>
#include <unordered_set>
#include <algorithm>

#include <imgui.h>
#include <glmlv/Image2DRGBA.hpp>
#include <glmlv/scene_loading.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>

int Application::run()
{
    float clearColor[3] = { 0, 0, 0 };
    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        const auto projMatrix = glm::perspective(70.f, float(m_nWindowWidth) / m_nWindowHeight, 0.01f * m_SceneSizeLength, m_SceneSizeLength);
        const auto viewMatrix = m_viewController.getViewMatrix();

        // Geometry pass
        {
            m_geometryPassProgram.use();
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_GBufferFBO);

            glViewport(0, 0, m_nWindowWidth, m_nWindowHeight);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            const auto modelMatrix = glm::mat4(1);

            const auto mvMatrix = viewMatrix * modelMatrix;
            const auto mvpMatrix = projMatrix * mvMatrix;
            const auto normalMatrix = glm::transpose(glm::inverse(mvMatrix));

            glUniformMatrix4fv(m_uModelViewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
            glUniformMatrix4fv(m_uModelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
            glUniformMatrix4fv(m_uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

            // Same sampler for all texture units
            glBindSampler(0, m_textureSampler);
            glBindSampler(1, m_textureSampler);
            glBindSampler(2, m_textureSampler);
            glBindSampler(3, m_textureSampler);

            // Set texture unit of each sampler
            glUniform1i(m_uKaSamplerLocation, 0);
            glUniform1i(m_uKdSamplerLocation, 1);
            glUniform1i(m_uKsSamplerLocation, 2);
            glUniform1i(m_uShininessSamplerLocation, 3);

            const auto bindMaterial = [&](const PhongMaterial & material)
            {
                glUniform3fv(m_uKaLocation, 1, glm::value_ptr(material.Ka));
                glUniform3fv(m_uKdLocation, 1, glm::value_ptr(material.Kd));
                glUniform3fv(m_uKsLocation, 1, glm::value_ptr(material.Ks));
                glUniform1fv(m_uShininessLocation, 1, &material.shininess);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, material.KaTextureId);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, material.KdTextureId);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, material.KsTextureId);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, material.shininessTextureId);
            };

            glBindVertexArray(m_SceneVAO);

            const PhongMaterial * currentMaterial = nullptr;

            // We draw each shape by specifying how much indices it carries, and with an offset in the global index buffer
            for (const auto shape : m_shapes)
            {
                const auto & material = shape.materialID >= 0 ? m_SceneMaterials[shape.materialID] : m_DefaultMaterial;
                if (currentMaterial != &material)
                {
                    bindMaterial(material);
                    currentMaterial = &material;
                }
                glDrawElements(GL_TRIANGLES, shape.indexCount, GL_UNSIGNED_INT, (const GLvoid*)(shape.indexOffset * sizeof(GLuint)));
            }

            for (GLuint i : {0, 1, 2, 3})
                glBindSampler(0, m_textureSampler);
        
            glBindVertexArray(0);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        }

        // Put here rendering code
        const auto viewportSize = m_GLFWHandle.framebufferSize();
        glViewport(0, 0, viewportSize.x, viewportSize.y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (m_CurrentlyDisplayed == GBufferTextureCount) // Beauty
        {
            // Shading pass
            {
                m_shadingPassProgram.use();

                glUniform3fv(m_uDirectionalLightDirLocation, 1, glm::value_ptr(glm::vec3(viewMatrix * glm::vec4(glm::normalize(m_DirLightDirection), 0))));
                glUniform3fv(m_uDirectionalLightIntensityLocation, 1, glm::value_ptr(m_DirLightColor * m_DirLightIntensity));

                glUniform3fv(m_uPointLightPositionLocation, 1, glm::value_ptr(glm::vec3(viewMatrix * glm::vec4(m_PointLightPosition, 1))));
                glUniform3fv(m_uPointLightIntensityLocation, 1, glm::value_ptr(m_PointLightColor * m_PointLightIntensity));

                for (int32_t i = GPosition; i < GDepth; ++i)
                {
                    glActiveTexture(GL_TEXTURE0 + i);
                    glBindTexture(GL_TEXTURE_2D, m_GBufferTextures[i]);

                    glUniform1i(m_uGBufferSamplerLocations[i], i);
                }

                glBindVertexArray(m_TriangleVAO);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                glBindVertexArray(0);
            }
        }
        else if (m_CurrentlyDisplayed == GDepth)
        {
            m_displayDepthProgram.use();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_GBufferTextures[GDepth]);

            glUniform1i(m_uGDepthSamplerLocation, 0);

            glBindVertexArray(m_TriangleVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
        }
        else if (m_CurrentlyDisplayed == GPosition)
        {
            m_displayPositionProgram.use();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_GBufferTextures[GPosition]);

            glUniform1i(m_uGDepthSamplerLocation, 0);

            const auto rcpProjMat = glm::inverse(projMatrix);

            const glm::vec4 frustrumTopRight(1, 1, 1, 1);
            const auto frustrumTopRight_view = rcpProjMat * frustrumTopRight;

            glUniform3fv(m_uSceneSizeLocation, 1, glm::value_ptr(frustrumTopRight_view / frustrumTopRight_view.w));

            glBindVertexArray(m_TriangleVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
        }
        else
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
            if (ImGui::Button("Sort shapes wrt materialID"))
            {
                std::sort(begin(m_shapes), end(m_shapes), [&](auto lhs, auto rhs)
                {
                    return lhs.materialID < rhs.materialID;
                });
            }
            if (ImGui::CollapsingHeader("Directional Light"))
            {
                ImGui::ColorEdit3("DirLightColor", glm::value_ptr(m_DirLightColor));
                ImGui::DragFloat("DirLightIntensity", &m_DirLightIntensity, 0.1f, 0.f, 100.f);
                if (ImGui::DragFloat("Phi Angle", &m_DirLightPhiAngleDegrees, 1.0f, 0.0f, 360.f) ||
                    ImGui::DragFloat("Theta Angle", &m_DirLightThetaAngleDegrees, 1.0f, 0.0f, 180.f)) {
                    m_DirLightDirection = computeDirectionVector(glm::radians(m_DirLightPhiAngleDegrees), glm::radians(m_DirLightThetaAngleDegrees));
                }
            }

            if (ImGui::CollapsingHeader("Point Light"))
            {
                ImGui::ColorEdit3("PointLightColor", glm::value_ptr(m_PointLightColor));
                ImGui::DragFloat("PointLightIntensity", &m_PointLightIntensity, 0.1f, 0.f, 16000.f);
                ImGui::InputFloat3("Position", glm::value_ptr(m_PointLightPosition));
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
    m_ShadersRootPath { m_AppPath.parent_path() / "shaders" }
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " < path to model >" << std::endl;
		exit(-1);
	}

    ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file

	initScene(glmlv::fs::path{ argv[1] });
    initShadersData();

    glEnable(GL_DEPTH_TEST);
    m_viewController.setSpeed(m_SceneSizeLength * 0.1f); // Let's travel 10% of the scene per second

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
}

void Application::initScene(const glmlv::fs::path & objPath)
{
    glGenBuffers(1, &m_SceneVBO);
    glGenBuffers(1, &m_SceneIBO);

    {
        glmlv::SceneData data;
        loadObjScene(objPath, data);
        m_SceneSize = data.bboxMax - data.bboxMin;
        m_SceneSizeLength = glm::length(m_SceneSize);

        std::cout << "# of shapes    : " << data.shapeCount << std::endl;
        std::cout << "# of materials : " << data.materials.size() << std::endl;
        std::cout << "# of vertex    : " << data.vertexBuffer.size() << std::endl;
        std::cout << "# of triangles    : " << data.indexBuffer.size() / 3 << std::endl;

        // Fill VBO
        glBindBuffer(GL_ARRAY_BUFFER, m_SceneVBO);
        glBufferStorage(GL_ARRAY_BUFFER, data.vertexBuffer.size() * sizeof(glmlv::Vertex3f3f2f), data.vertexBuffer.data(), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Fill IBO
        glBindBuffer(GL_ARRAY_BUFFER, m_SceneIBO);
        glBufferStorage(GL_ARRAY_BUFFER, data.indexBuffer.size() * sizeof(uint32_t), data.indexBuffer.data(), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Init shape infos
        uint32_t indexOffset = 0;
        for (auto shapeID = 0; shapeID < data.indexCountPerShape.size(); ++shapeID)
        {
            m_shapes.emplace_back();
            auto & shape = m_shapes.back();
            shape.indexCount = data.indexCountPerShape[shapeID];
            shape.indexOffset = indexOffset;
            shape.materialID = data.materialIDPerShape[shapeID];
            indexOffset += shape.indexCount;
        }

        glGenTextures(1, &m_WhiteTexture);
        glBindTexture(GL_TEXTURE_2D, m_WhiteTexture);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 1, 1);
        glm::vec4 white(1.f, 1.f, 1.f, 1.f);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGBA, GL_FLOAT, &white);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Upload all textures to the GPU
        std::vector<GLint> textureIds;
        for (const auto & texture : data.textures)
        {
            GLuint texId = 0;
            glGenTextures(1, &texId);
            glBindTexture(GL_TEXTURE_2D, texId);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, texture.width(), texture.height());
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture.width(), texture.height(), GL_RGBA, GL_UNSIGNED_BYTE, texture.data());
            glBindTexture(GL_TEXTURE_2D, 0);

            textureIds.emplace_back(texId);
        }

        for (const auto & material : data.materials)
        {
            PhongMaterial newMaterial;
            newMaterial.Ka = material.Ka;
            newMaterial.Kd = material.Kd;
            newMaterial.Ks = material.Ks;
            newMaterial.shininess = material.shininess;
            newMaterial.KaTextureId = material.KaTextureId >= 0 ? textureIds[material.KaTextureId] : m_WhiteTexture;
            newMaterial.KdTextureId = material.KdTextureId >= 0 ? textureIds[material.KdTextureId] : m_WhiteTexture;
            newMaterial.KsTextureId = material.KsTextureId >= 0 ? textureIds[material.KsTextureId] : m_WhiteTexture;
            newMaterial.shininessTextureId = material.shininessTextureId >= 0 ? textureIds[material.shininessTextureId] : m_WhiteTexture;

            m_SceneMaterials.emplace_back(newMaterial);
        }

        m_DefaultMaterial.Ka = glm::vec3(0);
        m_DefaultMaterial.Kd = glm::vec3(1);
        m_DefaultMaterial.Ks = glm::vec3(1);
        m_DefaultMaterial.shininess = 32.f;
        m_DefaultMaterial.KaTextureId = m_WhiteTexture;
        m_DefaultMaterial.KdTextureId = m_WhiteTexture;
        m_DefaultMaterial.KsTextureId = m_WhiteTexture;
        m_DefaultMaterial.shininessTextureId = m_WhiteTexture;
    }

    // Fill VAO
    glGenVertexArrays(1, &m_SceneVAO);
    glBindVertexArray(m_SceneVAO);

    const GLint positionAttrLocation = 0;
    const GLint normalAttrLocation = 1;
    const GLint texCoordsAttrLocation = 2;

    // We tell OpenGL what vertex attributes our VAO is describing:
    glEnableVertexAttribArray(positionAttrLocation);
    glEnableVertexAttribArray(normalAttrLocation);
    glEnableVertexAttribArray(texCoordsAttrLocation);

    glBindBuffer(GL_ARRAY_BUFFER, m_SceneVBO); // We bind the VBO because the next 3 calls will read what VBO is bound in order to know where the data is stored

    glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*)offsetof(glmlv::Vertex3f3f2f, position));
    glVertexAttribPointer(normalAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*)offsetof(glmlv::Vertex3f3f2f, normal));
    glVertexAttribPointer(texCoordsAttrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*)offsetof(glmlv::Vertex3f3f2f, texCoords));

    glBindBuffer(GL_ARRAY_BUFFER, 0); // We can unbind the VBO because OpenGL has "written" in the VAO what VBO it needs to read when the VAO will be drawn

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_SceneIBO); // Binding the IBO to GL_ELEMENT_ARRAY_BUFFER while a VAO is bound "writes" it in the VAO for usage when the VAO will be drawn

    glBindVertexArray(0);

    // Note: no need to bind a sampler for modifying it: the sampler API is already direct_state_access
    glGenSamplers(1, &m_textureSampler);
    glSamplerParameteri(m_textureSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(m_textureSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

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
    
    m_uDirectionalLightDirLocation = glGetUniformLocation(m_shadingPassProgram.glId(), "uDirectionalLightDir");
    m_uDirectionalLightIntensityLocation = glGetUniformLocation(m_shadingPassProgram.glId(), "uDirectionalLightIntensity");

    m_uPointLightPositionLocation = glGetUniformLocation(m_shadingPassProgram.glId(), "uPointLightPosition");
    m_uPointLightIntensityLocation = glGetUniformLocation(m_shadingPassProgram.glId(), "uPointLightIntensity");

    m_displayDepthProgram = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "shadingPass.vs.glsl", m_ShadersRootPath / m_AppName / "displayDepth.fs.glsl" });

    m_uGDepthSamplerLocation = glGetUniformLocation(m_displayDepthProgram.glId(), "uGDepth");

    m_displayPositionProgram = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "shadingPass.vs.glsl", m_ShadersRootPath / m_AppName / "displayPosition.fs.glsl" });

    m_uGPositionSamplerLocation = glGetUniformLocation(m_displayPositionProgram.glId(), "uGPosition");
    m_uSceneSizeLocation = glGetUniformLocation(m_displayPositionProgram.glId(), "uSceneSize");
}
