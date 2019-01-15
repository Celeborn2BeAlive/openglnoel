#include "Application.hpp"

#include <iostream>

#include <glmlv/Image2DRGBA.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>

int Application::run()
{
	float clearColor[3] = { 0.5, 0.8, 0.2 };
	glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);

    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        // Put here rendering code
        const auto viewportSize = m_GLFWHandle.framebufferSize();
        glViewport(0, 0, viewportSize.x, viewportSize.y);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const auto projMatrix = glm::perspective(70.f, float(viewportSize.x) / viewportSize.y, 0.01f, 100.f);
        const auto viewMatrix = m_viewController.getViewMatrix();

        glUniform3fv(m_uDirectionalLightDirLocation, 1, glm::value_ptr(glm::vec3(viewMatrix * glm::vec4(glm::normalize(m_DirLightDirection), 0))));
        glUniform3fv(m_uDirectionalLightIntensityLocation, 1, glm::value_ptr(m_DirLightColor * m_DirLightIntensity));

        glUniform3fv(m_uPointLightPositionLocation, 1, glm::value_ptr(glm::vec3(viewMatrix * glm::vec4(m_PointLightPosition, 1))));
        glUniform3fv(m_uPointLightIntensityLocation, 1, glm::value_ptr(m_PointLightColor * m_PointLightIntensity));

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(m_uKdSamplerLocation, 0); // Set the uniform to 0 because we use texture unit 0
        glBindSampler(0, m_textureSampler); // Tell to OpenGL what sampler we want to use on this texture unit

        {
            const auto modelMatrix = glm::rotate(glm::translate(glm::mat4(1), glm::vec3(-2, 0, 0)), 0.2f * float(seconds), glm::vec3(0, 1, 0));

            const auto mvMatrix = viewMatrix * modelMatrix;
            const auto mvpMatrix = projMatrix * mvMatrix;
            const auto normalMatrix = glm::transpose(glm::inverse(mvMatrix));

            glUniformMatrix4fv(m_uModelViewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
            glUniformMatrix4fv(m_uModelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
            glUniformMatrix4fv(m_uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

            glUniform3fv(m_uKdLocation, 1, glm::value_ptr(m_CubeKd));

            glBindTexture(GL_TEXTURE_2D, m_cubeTextureKd);

            glBindVertexArray(m_cubeVAO);
            glDrawElements(GL_TRIANGLES, m_cubeGeometry.indexBuffer.size(), GL_UNSIGNED_INT, nullptr);
        }

        {
            const auto modelMatrix = glm::rotate(glm::translate(glm::mat4(1), glm::vec3(2, 0, 0)), 0.2f * float(seconds), glm::vec3(0, 1, 0));

            const auto mvMatrix = viewMatrix * modelMatrix;
            const auto mvpMatrix = projMatrix * mvMatrix;
            const auto normalMatrix = glm::transpose(glm::inverse(mvMatrix));

            glUniformMatrix4fv(m_uModelViewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
            glUniformMatrix4fv(m_uModelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
            glUniformMatrix4fv(m_uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

            glUniform3fv(m_uKdLocation, 1, glm::value_ptr(m_SphereKd));

            glBindTexture(GL_TEXTURE_2D, m_sphereTextureKd);

            glBindVertexArray(m_sphereVAO);
            glDrawElements(GL_TRIANGLES, m_sphereGeometry.indexBuffer.size(), GL_UNSIGNED_INT, nullptr);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
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

            if (ImGui::CollapsingHeader("Materials"))
            {
                ImGui::ColorEdit3("Cube Kd", glm::value_ptr(m_CubeKd));
                ImGui::ColorEdit3("Sphere Kd", glm::value_ptr(m_SphereKd));
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

    const GLint vboBindingIndex = 0; // Arbitrary choice between 0 and glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS)

    const GLint positionAttrLocation = 0;
    const GLint normalAttrLocation = 1;
    const GLint texCoordsAttrLocation = 2;

    glGenBuffers(1, &m_cubeVBO);
    glGenBuffers(1, &m_cubeIBO);
    glGenBuffers(1, &m_sphereVBO);
    glGenBuffers(1, &m_sphereIBO);

    m_cubeGeometry = glmlv::makeCube();
    m_sphereGeometry = glmlv::makeSphere(32);

    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
    glBufferStorage(GL_ARRAY_BUFFER, m_cubeGeometry.vertexBuffer.size() * sizeof(glmlv::Vertex3f3f2f), m_cubeGeometry.vertexBuffer.data(), 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_sphereVBO);
    glBufferStorage(GL_ARRAY_BUFFER, m_sphereGeometry.vertexBuffer.size() * sizeof(glmlv::Vertex3f3f2f), m_sphereGeometry.vertexBuffer.data(), 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_cubeIBO);
    glBufferStorage(GL_ARRAY_BUFFER, m_cubeGeometry.indexBuffer.size() * sizeof(uint32_t), m_cubeGeometry.indexBuffer.data(), 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_sphereIBO);
    glBufferStorage(GL_ARRAY_BUFFER, m_sphereGeometry.indexBuffer.size() * sizeof(uint32_t), m_sphereGeometry.indexBuffer.data(), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Lets use a lambda to factorize VAO initialization:
    const auto initVAO = [positionAttrLocation, normalAttrLocation, texCoordsAttrLocation](GLuint& vao, GLuint vbo, GLuint ibo)
    {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // We tell OpenGL what vertex attributes our VAO is describing:
        glEnableVertexAttribArray(positionAttrLocation);
        glEnableVertexAttribArray(normalAttrLocation);
        glEnableVertexAttribArray(texCoordsAttrLocation);

        glBindBuffer(GL_ARRAY_BUFFER, vbo); // We bind the VBO because the next 3 calls will read what VBO is bound in order to know where the data is stored

        glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*)offsetof(glmlv::Vertex3f3f2f, position));
        glVertexAttribPointer(normalAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*)offsetof(glmlv::Vertex3f3f2f, normal));
        glVertexAttribPointer(texCoordsAttrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*)offsetof(glmlv::Vertex3f3f2f, texCoords));

        glBindBuffer(GL_ARRAY_BUFFER, 0); // We can unbind the VBO because OpenGL has "written" in the VAO what VBO it needs to read when the VAO will be drawn

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); // Binding the IBO to GL_ELEMENT_ARRAY_BUFFER while a VAO is bound "writes" it in the VAO for usage when the VAO will be drawn

        glBindVertexArray(0);
    };

    initVAO(m_cubeVAO, m_cubeVBO, m_cubeIBO);
    initVAO(m_sphereVAO, m_sphereVBO, m_sphereIBO);

    glActiveTexture(GL_TEXTURE0); // We will work on GL_TEXTURE0 texture unit. Since the shader only use one texture at a time, we only need one texture unit
    {
        glmlv::Image2DRGBA image = glmlv::readImage(m_AssetsRootPath / m_AppName / "textures" / "cube.png");

        glGenTextures(1, &m_cubeTextureKd);
        glBindTexture(GL_TEXTURE_2D, m_cubeTextureKd);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, image.width(), image.height());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width(), image.height(), GL_RGBA, GL_UNSIGNED_BYTE, image.data());
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    {
        glmlv::Image2DRGBA image = glmlv::readImage(m_AssetsRootPath / m_AppName / "textures" / "sphere.jpg");

        glGenTextures(1, &m_sphereTextureKd);
        glBindTexture(GL_TEXTURE_2D, m_sphereTextureKd);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, image.width(), image.height());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width(), image.height(), GL_RGBA, GL_UNSIGNED_BYTE, image.data());
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Note: no need to bind a sampler for modifying it: the sampler API is already direct_state_access
    glGenSamplers(1, &m_textureSampler);
    glSamplerParameteri(m_textureSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(m_textureSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glEnable(GL_DEPTH_TEST);

    m_program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "forward.vs.glsl", m_ShadersRootPath / m_AppName / "forward.fs.glsl" });
    m_program.use();

    m_uModelViewProjMatrixLocation = glGetUniformLocation(m_program.glId(), "uModelViewProjMatrix");
    m_uModelViewMatrixLocation = glGetUniformLocation(m_program.glId(), "uModelViewMatrix");
    m_uNormalMatrixLocation = glGetUniformLocation(m_program.glId(), "uNormalMatrix");


    m_uDirectionalLightDirLocation = glGetUniformLocation(m_program.glId(), "uDirectionalLightDir");
    m_uDirectionalLightIntensityLocation = glGetUniformLocation(m_program.glId(), "uDirectionalLightIntensity");

    m_uPointLightPositionLocation = glGetUniformLocation(m_program.glId(), "uPointLightPosition");
    m_uPointLightIntensityLocation = glGetUniformLocation(m_program.glId(), "uPointLightIntensity");

    m_uKdLocation = glGetUniformLocation(m_program.glId(), "uKd");
    m_uKdSamplerLocation = glGetUniformLocation(m_program.glId(), "uKdSampler");

    m_viewController.setViewMatrix(glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
}