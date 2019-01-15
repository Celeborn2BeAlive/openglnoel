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

	GLint m_uModelViewProjMatrixLocation;
	GLint m_uModelViewMatrixLocation;
	GLint m_uNormalMatrixLocation;

	glmlv::GLProgram program;
};