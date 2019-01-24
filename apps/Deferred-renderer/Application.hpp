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
	
	void initScene(const glmlv::fs::path & objPath);

	void initShadersData();

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


	enum GBufferTextureType
	{
		GPosition = 0,
		GNormal,
		GAmbient,
		GDiffuse,
		GGlossyShininess,
		GDepth, // On doit créer une texture de depth mais on écrit pas directement dedans dans le FS. OpenGL le fait pour nous (et l'utilise).
		GBufferTextureCount
	};

	const char * m_GBufferTexNames[GBufferTextureCount] = { "position", "normal", "ambient", "diffuse", "glossyShininess", "depth" };
	const GLenum m_GBufferTextureFormat[GBufferTextureCount] = { GL_RGB32F, GL_RGB32F, GL_RGB32F, GL_RGB32F, GL_RGBA32F, GL_DEPTH_COMPONENT32F };
	GLuint m_GBufferTextures[GBufferTextureCount];
	GLuint m_GBufferFBO; // Framebuffer object

	GBufferTextureType m_CurrentlyDisplayed = GDiffuse;

	//Creation of vao, vbo and ibo for obj model
	GLuint vaoObjModel = 0;
	GLuint vboObjModel = 0;
	GLuint iboObjModel = 0;

	// Required data about the scene in CPU in order to send draw calls
	struct ShapeInfo
	{
		uint32_t indexCount; // Number of indices
		uint32_t indexOffset; // Offset in GPU index buffer
		int materialID = -1;
		glm::mat4 localToWorldMatrix;
	};

	std::vector<ShapeInfo> m_shapes; // For each shape of the scene, its number of indices
	float m_SceneSize = 0.f; // Used for camera speed and projection matrix parameters

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

	GLuint textureSampler = 0; // Only one sampler object since we will use the same sampling parameters for all textures

	glmlv::ViewController viewController{ m_GLFWHandle.window(), 3.f };

	GLint uModelViewProjMatrixLocation;
	GLint uModelViewMatrixLocation;
	GLint uNormalMatrixLocation;

	glmlv::GLProgram program;

	GLint uKaLocation;
	GLint uKdLocation;
	GLint uKsLocation;
	GLint uShininessLocation;
	GLint uKaSamplerLocation;
	GLint uKdSamplerLocation;
	GLint uKsSamplerLocation;
	GLint uShininessSamplerLocation;

};