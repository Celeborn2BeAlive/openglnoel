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

	//Creation of vao, vbo and ibo for cube
	GLuint vaoCube = 0;
	GLuint vboCube = 0;
	GLuint iboCube = 0;

	//Creation of vao, vbo and ibo for sphere
	GLuint vaoSphere;
	GLuint vboSphere;
	GLuint iboSphere;

	//the cube
	glmlv::SimpleGeometry Cube;

	//the sphere
	glmlv::SimpleGeometry Sphere;

	GLsizei count;

	glmlv::ViewController viewController{ m_GLFWHandle.window(), 3.f };

	GLint uModelViewProjMatrixLocation;
	GLint uModelViewMatrixLocation;
	GLint uNormalMatrixLocation;

	glmlv::GLProgram program;

	
	GLint uDirectionalLightDirLocation;
	GLint uDirectionalLightIntensityLocation;

	GLint uPointLightPositionLocation;
	GLint uPointLightIntensityLocation;

	GLint uKdLocation;

	float DirLightPhiAngleDegrees = 90.f;
	float DirLightThetaAngleDegrees = 45.f;
	glm::vec3 DirLightDirection = computeDirectionVector(glm::radians(DirLightPhiAngleDegrees), glm::radians(DirLightThetaAngleDegrees));
	glm::vec3 DirLightColor = glm::vec3(1, 1, 1);
	float DirLightIntensity = 1.f;

	glm::vec3 PointLightPosition = glm::vec3(0, 1, 0);
	glm::vec3 PointLightColor = glm::vec3(1, 1, 1);
	float m_PointLightIntensity = 5.f;

	glm::vec3 CubeKd = glm::vec3(1, 0, 0);
	glm::vec3 SphereKd = glm::vec3(0, 1, 0);
	
};