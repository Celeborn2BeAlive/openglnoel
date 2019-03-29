#include "Application.hpp"

#include <iostream>

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
	float clearColor[3] = { 0.5, 0.8, 0.2 };
	glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);

    // Loop until the user closes the windows
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        // Put here rendering code
        const auto viewportSize = m_GLFWHandle.framebufferSize();
        glViewport(0, 0, viewportSize.x, viewportSize.y);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
            m_projMatrix = glm::perspective(70.f, float(viewportSize.x) / viewportSize.y, 0.01f, 100.f);            
        }
        
        m_viewMatrix = m_viewController.getViewMatrix();

        // LIGHT
        glUniform3fv(m_uDirectionalLightDirLocation, 1, glm::value_ptr(glm::vec3(m_viewMatrix * glm::vec4(glm::normalize(m_DirLightDirection), 0))));
        glUniform3fv(m_uDirectionalLightIntensityLocation, 1, glm::value_ptr(m_DirLightColor * m_DirLightIntensity));

        glUniform3fv(m_uPointLightPositionLocation, 1, glm::value_ptr(glm::vec3(m_viewMatrix * glm::vec4(m_PointLightPosition, 1))));
        glUniform3fv(m_uPointLightIntensityLocation, 1, glm::value_ptr(m_PointLightColor * m_PointLightIntensity));

        // ACTIVE TEXTURE
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(m_uKdSamplerLocation, 0); // Set the uniform to 0 because we use texture unit 0
        glBindSampler(0, m_textureSampler); // Tell to OpenGL what sampler we want to use on this texture unit

        // GLTF DRAWING
        {
            DrawModel(m_model);
        }

        // UNBIND        
        glBindSampler(0, 0); // Unbind the sampler

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

    // 1 - LINK WITH SHADERS

    // Attribute location
    const GLint positionAttrLocation = 0;
    const GLint normalAttrLocation = 1;
    const GLint texCoordsAttrLocation = 2;
    
    // Map gltf attributes name to an index value of vs
    m_attribs["POSITION"] = positionAttrLocation;
    m_attribs["NORMAL"] = normalAttrLocation;
    m_attribs["TEXCOORD_0"] = texCoordsAttrLocation;

    glEnable(GL_DEPTH_TEST);

    // Compile Shaders
    m_program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "forward.vs.glsl", m_ShadersRootPath / m_AppName / "forward.fs.glsl" });
    m_program.use();

    // Get Uniforms
    m_uModelViewProjMatrixLocation = glGetUniformLocation(m_program.glId(), "uModelViewProjMatrix");
    m_uModelViewMatrixLocation = glGetUniformLocation(m_program.glId(), "uModelViewMatrix");
    m_uNormalMatrixLocation = glGetUniformLocation(m_program.glId(), "uNormalMatrix");

    m_uDirectionalLightDirLocation = glGetUniformLocation(m_program.glId(), "uDirectionalLightDir");
    m_uDirectionalLightIntensityLocation = glGetUniformLocation(m_program.glId(), "uDirectionalLightIntensity");

    m_uPointLightPositionLocation = glGetUniformLocation(m_program.glId(), "uPointLightPosition");
    m_uPointLightIntensityLocation = glGetUniformLocation(m_program.glId(), "uPointLightIntensity");

    m_uKdLocation = glGetUniformLocation(m_program.glId(), "uKd");
    m_uKdSamplerLocation = glGetUniformLocation(m_program.glId(), "uKdSampler");

    // 2 - LOAD SCENE (.GLTF)
    if (argc < 2) {
        printf("Needs input.gltf\n");
        exit(1);
    }
    const glmlv::fs::path gltfPath = m_AssetsRootPath / m_AppName / glmlv::fs::path{ argv[1] };

    loadTinyGLTF(gltfPath);

    // 3 - SET CAMERA POSTION
    m_viewController.setViewMatrix(glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));    
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
    //m_buffers.resize(m_model.buffers.size());
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

            // INDICES (IBO)
            tinygltf::Accessor &indexAccessor = m_model.accessors[primitive.indices];
            tinygltf::BufferView &bufferView = m_model.bufferViews[indexAccessor.bufferView];
            int &bufferIndex = bufferView.buffer;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[bufferIndex]); // Binding IBO
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexAccessor.bufferView); // Binding IBO

            // ATTRIBUTS
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
                //glBindBuffer(GL_ARRAY_BUFFER, accessor.bufferView);    // Binding VBO
                
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
                if ((it->first.compare("POSITION") == 0) ||
                    (it->first.compare("NORMAL") == 0) ||
                    (it->first.compare("TEXCOORD_0") == 0))
                {
                    if (m_attribs[it->first] >= 0)
                    {
                        // Compute byteStride from Accessor + BufferView combination.
                        int byteStride =  accessor.ByteStride(m_model.bufferViews[accessor.bufferView]);
                        assert(byteStride != -1);
                        glEnableVertexAttribArray(m_attribs[it->first]);
                        glVertexAttribPointer(m_attribs[it->first], size, accessor.componentType, accessor.normalized ? GL_TRUE : GL_FALSE, byteStride, (const GLvoid*) (bufferView.byteOffset + accessor.byteOffset));
                    }
                }

                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }

            // On rempli le vao et les primitives
            meshInfos.vaos.push_back(vaoId);
            meshInfos.primitives.push_back(primitive);

            glBindVertexArray(0);

            // TEXTURES
            meshInfos.diffuseTex.push_back(0);

            if (primitive.material < 0) {
					continue;
            }

            tinygltf::Material &mat = m_model.materials[primitive.material];            

            if (mat.values.find("baseColorTexture") != mat.values.end())
            {
                const auto& parameter = mat.values["baseColorTexture"];

                tinygltf::Texture &tex = m_model.textures[parameter.TextureIndex()];

                if (tex.source > -1 && tex.source < m_model.images.size())
                {
                    tinygltf::Image &image = m_model.images[tex.source];

                    glActiveTexture(GL_TEXTURE0);

                    GLuint texId;
                    glGenTextures(1, &texId);
                    glBindTexture(GL_TEXTURE_2D, texId);

                    // Ignore Texture.fomat.
                    GLenum format = GL_RGBA;
                    if (image.component == 3) {
                        format = GL_RGB;
                    }

    /*
                    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                    glTexParameterf(tex.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameterf(tex.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                    glTexImage2D(tex.target, 0, tex.internalFormat, image.width,
                            image.height, 0, format, tex.type,
                            &image.image.at(0));
    */

                    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, image.width, image.height);
                    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width, image.height, GL_RGBA, GL_UNSIGNED_BYTE, &image.image.at(0));
                    
                    glBindTexture(GL_TEXTURE_2D, 0);
                    
                    meshInfos.diffuseTex.back() = texId;
                }
            }
        }

        // VSCode signal an error here but there's no error
        m_meshInfos.push_back(meshInfos);
    }

    // SAMPLER --> TODO
    // Note: no need to bind a sampler for modifying it: the sampler API is already direct_state_access
    glGenSamplers(1, &m_textureSampler);
    glSamplerParameteri(m_textureSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(m_textureSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //std::cout << m_model.cameras.size() << std::endl;
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

// ------ GLTF DRAW --------


void Application::DrawModel(tinygltf::Model &model) {
  // If the glTF asset has at least one scene, and doesn't define a default one
  // just show the first one we can find
  assert(model.scenes.size() > 0);
  int scene_to_display = model.defaultScene > -1 ? model.defaultScene : 0;
  const tinygltf::Scene &scene = model.scenes[scene_to_display];
  for (size_t i = 0; i < scene.nodes.size(); i++) 
  {
    DrawNode(model, model.nodes[scene.nodes[i]], glm::mat4(1));
  }
}

// Hierarchically draw nodes
void Application::DrawNode(tinygltf::Model &model, const tinygltf::Node &node, glm::mat4 currentMatrix) {

    // PUSH MATRIX
    glm::mat4 modelMatrix = glm::mat4(1);

    // Use `matrix' attribute
    if (node.matrix.size() == 16)
    {
        modelMatrix = glm::make_mat4(node.matrix.data());
    }
    // Use `translate', 'rotate' and 'scale' attributes
    else
    {       
        if (node.translation.size() == 3)
        {
            glm::vec3 translate = glm::make_vec3(node.translation.data());
            modelMatrix = glm::translate(modelMatrix, translate);
        }

        if (node.rotation.size() == 4)
        {
            //glm::mat4 rotMatrix = quatToMatrix(glm::make_vec4(node.rotation.data()));
            //float angle = rotMatrix[15];
            //glm::vec3 rotate(node.rotation[0], node.rotation[1], node.rotation[2]);
            //modelMatrix = glm::rotate(modelMatrix, (float)node.rotation[3], rotate);
            //std::cout << "BEFORE ROTATION" << std::endl;
            //std::cout << modelMatrix << std::endl;
            modelMatrix = quatToMatrix(glm::make_vec4(node.rotation.data())) * modelMatrix;
            //std::cout << "AFTER ROTATION" << std::endl;
            //std::cout << modelMatrix << std::endl;
        }

        if (node.scale.size() == 3)
        {
            glm::vec3 scale = glm::make_vec3(node.scale.data());
            modelMatrix = glm::scale(modelMatrix, scale);
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
        // Color
        glUniform3fv(m_uKdLocation, 1, glm::value_ptr(glm::vec3(1, 1, 1)));
        glBindTexture(GL_TEXTURE_2D, m_meshInfos[meshIndex].diffuseTex[i]);

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
