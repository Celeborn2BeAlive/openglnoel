#pragma once

#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/ViewController.hpp>
#include <glmlv/simple_geometry.hpp>

#include <glm/glm.hpp>

#include <limits>

class Application
{
public:
    Application(int argc, char** argv);

    int run();
private:
    void initScene();

    void initShadersData();

    void initGBuffer();

    void initScreenTriangle();

    void initShadowData();

    static glm::vec3 computeDirectionVector(float phiRadians, float thetaRadians)
    {
        const auto cosPhi = glm::cos(phiRadians);
        const auto sinPhi = glm::sin(phiRadians);
        const auto sinTheta = glm::sin(thetaRadians);
        return glm::vec3(sinPhi * sinTheta, glm::cos(thetaRadians), cosPhi * sinTheta);
    }

    static glm::vec3 computeDirectionVectorUp(float phiRadians, float thetaRadians)
    {
        const auto cosPhi = glm::cos(phiRadians);
        const auto sinPhi = glm::sin(phiRadians);
        const auto cosTheta = glm::cos(thetaRadians);
        return -glm::normalize(glm::vec3(sinPhi * cosTheta, -glm::sin(thetaRadians), cosPhi * cosTheta));
    }

    const size_t m_nWindowWidth = 1280;
    const size_t m_nWindowHeight = 720;
    glmlv::GLFWHandle m_GLFWHandle{ m_nWindowWidth, m_nWindowHeight, "Shadow Mapping - 01 - Directional Shadow Map" }; // Note: the handle must be declared before the creation of any object managing OpenGL resource (e.g. GLProgram, GLShader)

    const glmlv::fs::path m_AppPath;
    const std::string m_AppName;
    const std::string m_ImGuiIniFilename;
    const glmlv::fs::path m_ShadersRootPath;
    const glmlv::fs::path m_AssetsRootPath;

    // GBuffer:
    enum GBufferTextureType
    {
        GPosition = 0,
        GNormal,
        GAmbient,
        GDiffuse,
        GGlossyShininess,
        GDepth,
        GBufferTextureCount
    };

    const GLenum m_GBufferTextureFormat[GBufferTextureCount] = { GL_RGB32F, GL_RGB32F, GL_RGB32F, GL_RGB32F, GL_RGBA32F, GL_DEPTH_COMPONENT32F };
    GLuint m_GBufferTextures[GBufferTextureCount];
    GLuint m_GBufferFBO; // Framebuffer object

    enum DisplayType
    {
        Display_Beauty,
        Display_GPosition,
        Display_GNormal,
        Display_GAmbient,
        Display_GDiffuse,
        Display_GGlossyShininess,
        Display_GDepth,
        Display_DirectionalLightDepthMap,
        Display_PointLightDepthMap_PosX,
        Display_PointLightDepthMap_NegX,
        Display_PointLightDepthMap_PosY,
        Display_PointLightDepthMap_NegY,
        Display_PointLightDepthMap_PosZ,
        Display_PointLightDepthMap_NegZ,
        Display_Count
    };

    const char * m_DisplayNames[Display_Count] = { "beauty", "position", "normal", "ambient", "diffuse", "glossyShininess", "depth", "directionalLightDepth",
        "pointLightDepth_PosX", "pointLightDepth_NegX", "pointLightDepth_PosY", "pointLightDepth_NegY", "pointLightDepth_PosZ", "pointLightDepth_NegZ" };
    int32_t m_CurrentlyDisplayed = Display_Beauty;

    // Triangle covering the whole screen, for the shading pass:
    GLuint m_TriangleVBO = 0;
    GLuint m_TriangleVAO = 0;

    // Scene data in GPU:
    GLuint m_SceneVBO = 0;
    GLuint m_SceneIBO = 0;
    GLuint m_SceneVAO = 0;

    // Required data about the scene in CPU in order to send draw calls
    struct ShapeInfo
    {
        uint32_t indexCount; // Number of indices
        uint32_t indexOffset; // Offset in GPU index buffer
        int materialID = -1;
    };

    std::vector<ShapeInfo> m_shapes; // For each shape of the scene, its number of indices
    glm::vec3 m_SceneSize = glm::vec3(0.f); // Used for camera speed and projection matrix parameters
    float m_SceneSizeLength = 0.f;
    glm::vec3 m_SceneCenter = glm::vec3(0.f);

    struct PhongMaterial
    {
        glm::vec3 Ka = glm::vec3(0); // Ambient multiplier
        glm::vec3 Kd = glm::vec3(0); // Diffuse multiplier
        glm::vec3 Ks = glm::vec3(0); // Glossy multiplier
        float shininess = 1.f; // Glossy exponent

        // OpenGL texture ids:
        GLuint KaTextureId = 0;
        GLuint KdTextureId = 0;
        GLuint KsTextureId = 0;
        GLuint shininessTextureId = 0;
    };

    GLuint m_WhiteTexture; // A white 1x1 texture
    PhongMaterial m_DefaultMaterial;
    std::vector<PhongMaterial> m_SceneMaterials;

    GLuint m_textureSampler = 0; // Only one sampler object since we will use the same sampling parameters for all textures

    // Camera
    glmlv::ViewController m_viewController{ m_GLFWHandle.window(), 3.f };

    // GLSL programs
    glmlv::GLProgram m_geometryPassProgram;
    glmlv::GLProgram m_shadingPassProgram;
    glmlv::GLProgram m_displayDepthProgram;
    glmlv::GLProgram m_displayPositionProgram;

    // Geometry pass uniforms
    GLint m_uModelViewProjMatrixLocation;
    GLint m_uModelViewMatrixLocation;
    GLint m_uNormalMatrixLocation;
    GLint m_uKaLocation;
    GLint m_uKdLocation;
    GLint m_uKsLocation;
    GLint m_uShininessLocation;
    GLint m_uKaSamplerLocation;
    GLint m_uKdSamplerLocation;
    GLint m_uKsSamplerLocation;
    GLint m_uShininessSamplerLocation;

    // Shading pass uniforms
    GLint m_uGBufferSamplerLocations[GDepth];
    GLint m_uDirectionalLightDirLocation;
    GLint m_uDirectionalLightIntensityLocation;
    GLint m_uPointLightPositionLocation;
    GLint m_uPointLightIntensityLocation;
    GLint m_uDirLightViewProjMatrix_shadingPass; // Suffix because the variable m_uDirLightViewProjMatrix is already used for the uniform of m_directionalSMProgram.
    GLint m_uDirLightShadowMap;
    GLint m_uDirLightShadowMapBias;
    GLint m_uDirLightShadowMapSampleCount;
    GLint m_uDirLightShadowMapSpread;
    GLint m_uPointLightViewMatrix;
    GLint m_uPointLightViewProjMatrix_shadingPass; // Suffix because the variable m_uPointLightViewProjMatrix is already used for the uniform of m_directionalSMProgram.
    GLint m_uPointLightShadowMap;
    GLint m_uPointLightShadowMapBias;

    // Display depth pass uniforms
    GLint m_uGDepthSamplerLocation;

    // Display position pass uniforms
    GLint m_uGPositionSamplerLocation;
    GLint m_uSceneSizeLocation;

    // Lights
    float m_DirLightPhiAngleDegrees = 100.f;
    float m_DirLightThetaAngleDegrees = 30.f;
    glm::vec3 m_DirLightDirection = computeDirectionVector(glm::radians(m_DirLightPhiAngleDegrees), glm::radians(m_DirLightThetaAngleDegrees));
    glm::vec3 m_DirLightColor = glm::vec3(1, 1, 1);
    float m_DirLightIntensity = 0.f;
    float m_DirLightSMBias = 0.01f;
    int m_DirLightSMSampleCount = 16;
    float m_DirLightSMSpread = 0.0005;

    glm::vec3 m_PointLightPosition = glm::vec3(0, 200, 0);
    glm::vec3 m_PointLightColor = glm::vec3(1, 1, 1);
    float m_PointLightIntensity = 100000.000;

    // Shadow mapping data
    glmlv::GLProgram m_directionalSMProgram;
    GLint m_uDirLightViewProjMatrix;

    GLuint m_directionalSMSampler;

    GLuint m_directionalSMFBO;
    GLuint m_directionalSMTexture;
    int32_t m_nDirectionalSMResolution = 4096;

    glmlv::GLProgram m_pointSMProgram;
    GLint m_uPointLightViewProjMatrix;

    GLuint m_pointSMSampler;

    GLuint m_pointSMFBO;
    GLuint m_pointSMTexture;
    GLuint m_pointSMTextureView[6]; // Juste for display of one layer of the cube depth map
    int32_t m_nPointSMResolution = 512;
};