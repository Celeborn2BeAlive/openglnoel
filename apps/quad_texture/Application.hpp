#pragma once

#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>

class Application
{
public:
    Application(int argc, char** argv);

    ~Application();

    int run();
private:
    const size_t m_nWindowWidth = 1280;
    const size_t m_nWindowHeight = 720;
    glmlv::GLFWHandle m_GLFWHandle{ m_nWindowWidth, m_nWindowHeight, "Quad" }; // Note: the handle must be declared before the creation of any object managing OpenGL resource (e.g. GLProgram, GLShader)

    const glmlv::fs::path m_AppPath;
    const std::string m_AppName;
    const std::string m_ImGuiIniFilename;
    const glmlv::fs::path m_ShadersRootPath;
    const glmlv::fs::path m_AssetsRootPath;

    GLuint m_quadVBO = 0;
    GLuint m_quadIBO = 0;
    GLuint m_quadVAO = 0;

    GLuint m_texObject = 0;
    GLuint m_samplerObject = 0;

    GLint m_uSamplerLocation = -1;

    glmlv::GLProgram m_program;
};