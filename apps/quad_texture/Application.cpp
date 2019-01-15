#include "Application.hpp"

#include <iostream>

#include <glm/glm.hpp>
#include <imgui.h>
#include <glmlv/Image2DRGBA.hpp>

int Application::run()
{
    float clearColor[3] = { 0, 0, 0 };
    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT);

        // Put here rendering code

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(m_uSamplerLocation, 0); // Set the uniform to 0 because we use texture unit 0
        glBindSampler(0, m_samplerObject); // Tell to OpenGL what sampler we want to use on this texture unit

        glBindTexture(GL_TEXTURE_2D, m_texObject);
        glBindVertexArray(m_quadVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); // We draw 2 triangles for a quad, so 3 * 2 = 6 indices must be used
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

struct Vertex
{
    glm::vec2 position;
    glm::vec2 texCoords;

    Vertex(glm::vec2 position, glm::vec2 texCoords):
        position(position), texCoords(texCoords)
    {}
};

Application::Application(int argc, char** argv):
    m_AppPath{ glmlv::fs::path{ argv[0] } },
    m_AppName{ m_AppPath.stem().string() },
    m_ImGuiIniFilename{ m_AppName + ".imgui.ini" },
    m_ShadersRootPath{ m_AppPath.parent_path() / "shaders" },
    m_AssetsRootPath{ m_AppPath.parent_path() / "assets" }
{
    glGenBuffers(1, &m_quadVBO);

    Vertex quadVertices[] = {
        Vertex { glm::vec2(-0.5, -0.5), glm::vec2(0, 1) },
        Vertex { glm::vec2(0.5, -0.5), glm::vec2(1, 1) },
        Vertex { glm::vec2(0.5, 0.5), glm::vec2(1, 0) },
        Vertex { glm::vec2(-0.5, 0.5), glm::vec2(0, 0) }
    };

    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);

    glBufferStorage(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_quadIBO);

    glBindBuffer(GL_ARRAY_BUFFER, m_quadIBO);

    GLuint quadIndices[] = {
        0, 1, 2, // First triangle
        0, 2, 3 // Second triangle
    };

    glBufferStorage(GL_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &m_quadVAO);

    // Here we load and compile shaders from the library
    m_program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "texture.vs.glsl", m_ShadersRootPath / m_AppName / "texture.fs.glsl" });

    // Here we use glGetAttribLocation(program, attribname) to obtain attrib locations; We could also directly use locations if they are set in the vertex shader (cf. triangle app)
    const GLint positionAttrLocation = glGetAttribLocation(m_program.glId(), "aPosition");
    const GLint texCoordsAttrLocation = glGetAttribLocation(m_program.glId(), "aTexCoords");

    glBindVertexArray(m_quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);

    glEnableVertexAttribArray(positionAttrLocation);
    glVertexAttribPointer(positionAttrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*) offsetof(Vertex, position));

    glEnableVertexAttribArray(texCoordsAttrLocation);
    glVertexAttribPointer(texCoordsAttrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*) offsetof(Vertex, texCoords));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quadIBO);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    {
        auto image = glmlv::readImage(m_AssetsRootPath / m_AppName / "textures" / "opengl-logo.png");

        glActiveTexture(GL_TEXTURE0);

        glGenTextures(1, &m_texObject);
        glBindTexture(GL_TEXTURE_2D, m_texObject);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, image.width(), image.height());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width(), image.height(), GL_RGBA, GL_UNSIGNED_BYTE, image.data());
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Note: no need to bind a sampler for modifying it: the sampler API is already direct_state_access
    glGenSamplers(1, &m_samplerObject);
    glSamplerParameteri(m_samplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(m_samplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_uSamplerLocation = glGetUniformLocation(m_program.glId(), "uSampler");

    m_program.use();
}

Application::~Application()
{
    if (m_quadVBO) {
        glDeleteBuffers(1, &m_quadVBO);
    }

    if (m_quadIBO) {
        glDeleteBuffers(1, &m_quadIBO);
    }

    if (m_quadVAO) {
        glDeleteBuffers(1, &m_quadVAO);
    }

    if (m_texObject) {
        glDeleteTextures(1, &m_texObject);
    }

    if (m_samplerObject) {
        glDeleteSamplers(1, &m_samplerObject);
    }

    glfwTerminate();
}
