#pragma once

#include <map>

#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/ViewController.hpp>
#include <glmlv/simple_geometry.hpp>

#include <glm/glm.hpp>

#include <tiny_gltf.h>

class Application
{
public:
    Application(int argc, char** argv);

    int run();
private:
    static glm::vec3 computeDirectionVector(float phiRadians, float thetaRadians)
    {
        const auto cosPhi = glm::cos(phiRadians);
        const auto sinPhi = glm::sin(phiRadians);
        const auto sinTheta = glm::sin(thetaRadians);
        return glm::vec3(sinPhi * sinTheta, glm::cos(thetaRadians), cosPhi * sinTheta);
    }

    const size_t m_nWindowWidth = 1280;
    const size_t m_nWindowHeight = 720;
    glmlv::GLFWHandle m_GLFWHandle{ m_nWindowWidth, m_nWindowHeight, "Template" }; // Note: the handle must be declared before the creation of any object managing OpenGL resource (e.g. GLProgram, GLShader)

    const glmlv::fs::path m_AppPath;
    const std::string m_AppName;
    const std::string m_ImGuiIniFilename;
    const glmlv::fs::path m_ShadersRootPath;
    const glmlv::fs::path m_AssetsRootPath;

    GLuint m_textureSampler = 0; // Only one sampler object since we will use the sample sampling parameters for the two textures

    glmlv::GLProgram m_program;

    glmlv::ViewController m_viewController{ m_GLFWHandle.window(), 3.f };
    GLint m_uModelViewProjMatrixLocation;
    GLint m_uModelViewMatrixLocation;
    GLint m_uNormalMatrixLocation;

    GLint m_uDirectionalLightDirLocation;
    GLint m_uDirectionalLightIntensityLocation;

    GLint m_uPointLightPositionLocation;
    GLint m_uPointLightIntensityLocation;

    GLint m_uKdLocation;
    GLint m_uKdSamplerLocation;

    float m_DirLightPhiAngleDegrees = 330.f;
    float m_DirLightThetaAngleDegrees = 150.f;
    glm::vec3 m_DirLightDirection = computeDirectionVector(glm::radians(m_DirLightPhiAngleDegrees), glm::radians(m_DirLightThetaAngleDegrees));
    glm::vec3 m_DirLightColor = glm::vec3(1, 1, 1);
    float m_DirLightIntensity = 1.f;

    glm::vec3 m_PointLightPosition = glm::vec3(0, 1, 0);
    glm::vec3 m_PointLightColor = glm::vec3(1, 1, 1);
    float m_PointLightIntensity = 5.f;
    
    glm::mat4 m_projMatrix;
    glm::mat4 m_viewMatrix;
    glm::mat4 quatToMatrix(glm::vec4 quaternion);

    // For GLTF
    tinygltf::Model m_model;
    std::map<std::string, GLint> m_attribs;

    // TODO --> Maybe We can put all 3 into a structure because the same Index means the same element
    std::vector<GLuint> m_vaos;
    std::vector<tinygltf::Primitive> m_primitives;
    std::vector<GLuint> m_diffuseTex;

    void loadTinyGLTF(const glmlv::fs::path & gltfPath);
    void drawGLTF();
    GLenum getMode(int mode);

    void DrawModel(tinygltf::Model &model);
    void DrawNode(tinygltf::Model &model, const tinygltf::Node &node, glm::mat4 currentMatrix);
    void DrawMesh(int meshIndex, glm::mat4 modelMatrix);
};