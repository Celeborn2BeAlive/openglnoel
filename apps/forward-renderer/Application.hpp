#pragma once

#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/ViewController.hpp>
#include <glmlv/simple_geometry.hpp>

class Application
{
public:
    Application(int argc, char** argv);

    int run();
private:

    //POUR LA FREEFLY CAMERA
    static glm::vec3 computeDirectionVector(float phiRadians, float thetaRadians)
    {
        const auto cosPhi = glm::cos(phiRadians);
        const auto sinPhi = glm::sin(phiRadians);
        const auto sinTheta = glm::sin(thetaRadians);
        return glm::vec3(sinPhi * sinTheta, glm::cos(thetaRadians), cosPhi * sinTheta);
    }

    //TAILLE FENETRES
    const size_t m_nWindowWidth = 800;
    const size_t m_nWindowHeight = 800;
    glmlv::GLFWHandle m_GLFWHandle{ m_nWindowWidth, m_nWindowHeight, "Template" }; // Note: the handle must be declared before the creation of any object managing OpenGL resource (e.g. GLProgram, GLShader)

    //POUR LES PATHS
    const glmlv::fs::path m_AppPath;
    const std::string m_AppName;
    const std::string m_ImGuiIniFilename;
    const glmlv::fs::path m_ShadersRootPath;
    const glmlv::fs::path m_AssetsRootPath;

    //PROGRAM
    glmlv::GLProgram program;

    //SPHERE
    glmlv::SimpleGeometry m_sphereGeometry;

    GLuint m_sphereVBO = 0;
    GLuint m_sphereIBO = 0;
    GLuint m_sphereVAO = 0;
    GLuint m_sphereTXO = 0;

    GLuint loc_MVP; 
    GLuint loc_MV;
    GLuint loc_N;

    GLint loc_tex;

    GLuint loc_DLD;
    GLuint loc_DLI;
    GLuint loc_PLP;
    GLuint loc_PLI;
    GLuint loc_KD;

    float m_DirLightPhiAngleDegrees = 90.f;
    float m_DirLightThetaAngleDegrees = 45.f;
    glm::vec3 m_DirLightDirection = computeDirectionVector(glm::radians(m_DirLightPhiAngleDegrees), glm::radians(m_DirLightThetaAngleDegrees));
    glm::vec3 m_DirLightColor = glm::vec3(1, 1, 1);
    float m_DirLightIntensity = 1.f;
    glm::vec3 m_PointLightPosition = glm::vec3(0, 1, 0);
    glm::vec3 m_PointLightColor = glm::vec3(1, 1, 1);
    float m_PointLightIntensity = 5.f;

    glm::vec3 m_SphereKd = glm::vec3(0, 1, 0);

    glmlv::ViewController m_viewController{ m_GLFWHandle.window(), 3.f };
};
