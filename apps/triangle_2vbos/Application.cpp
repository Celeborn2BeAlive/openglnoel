#include "Application.hpp"

#include <iostream>

#include <glm/glm.hpp>
#include <imgui.h>

int Application::run()
{
    float clearColor[3] = { 0, 0, 0 };
    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT);

        // Put here rendering code
        glBindVertexArray(m_triangleVAO);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);

        // GUI code:
		glmlv::imguiNewFrame();

        {
            ImGui::Begin("GUI");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            if (ImGui::ColorEdit3("clearColor", clearColor)) {
                glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);
            }
            ImGui::End();
        }

        const auto viewportSize = m_GLFWHandle.framebufferSize();
        glViewport(0, 0, viewportSize.x, viewportSize.y);
        
		glmlv::imguiRenderFrame();

        /* Poll for and process events */
        glfwPollEvents();

        /* Swap front and back buffers*/
        m_GLFWHandle.swapBuffers();

        auto ellapsedTime = glfwGetTime() - seconds;
        auto guiHasFocus = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
        if (!guiHasFocus) {
            //viewController.update(float(ellapsedTime))
        }
    }

    return 0;
}

Application::Application(int argc, char** argv):
    m_AppPath{ glmlv::fs::path{ argv[0] } },
    m_AppName{ m_AppPath.stem().string() },
    m_ImGuiIniFilename{ m_AppName + ".imgui.ini" },
    m_ShadersRootPath{ m_AppPath.parent_path() / "shaders" }
{
    glGenBuffers(1, &m_trianglePositionsVBO);

    glm::vec2 trianglePositions[] = {
        glm::vec2(-0.5, -0.5),
        glm::vec2(0.5, -0.5),
        glm::vec2(0, 0.5),
    };

    glBindBuffer(GL_ARRAY_BUFFER, m_trianglePositionsVBO);

    glBufferStorage(GL_ARRAY_BUFFER, sizeof(trianglePositions), trianglePositions, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glGenBuffers(1, &m_triangleColorsVBO);

    glm::vec3 triangleColors[] = {
        glm::vec3(1, 0, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 0, 1)
    };

    glBindBuffer(GL_ARRAY_BUFFER, m_triangleColorsVBO);

    glBufferStorage(GL_ARRAY_BUFFER, sizeof(triangleColors), triangleColors, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glGenVertexArrays(1, &m_triangleVAO);

    // Vertex attrib locations are defined in the vertex shader (we can also use glGetAttribLocation(program, attribname) with attribute names after program compilation in order to get these numbers)
    const GLint positionAttrLocation = 4;
    const GLint colorAttrLocation = 2;

    glBindVertexArray(m_triangleVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_trianglePositionsVBO);

    glEnableVertexAttribArray(positionAttrLocation);
    glVertexAttribPointer(positionAttrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_triangleColorsVBO);

    glEnableVertexAttribArray(colorAttrLocation);
    glVertexAttribPointer(colorAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    m_program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "triangle.vs.glsl", m_ShadersRootPath / m_AppName / "triangle.fs.glsl" });
    m_program.use();
}

Application::~Application()
{
    if (m_trianglePositionsVBO) {
        glDeleteBuffers(1, &m_trianglePositionsVBO);
    }

    if (m_triangleColorsVBO) {
        glDeleteBuffers(1, &m_triangleColorsVBO);
    }

    if (m_triangleVAO) {
        glDeleteBuffers(1, &m_triangleVAO);
    }

    glfwTerminate();
}
