#include "Application.hpp"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int Application::run()
{


	glBindVertexArray(vao);

	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        // Put here rendering code
		const auto fbSize = m_GLFWHandle.framebufferSize();
		glViewport(0, 0, fbSize.x, fbSize.y);
		glClear(GL_COLOR_BUFFER_BIT);

        // GUI code:
		glmlv::imguiNewFrame();

        {
            ImGui::Begin("GUI");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

		glmlv::imguiRenderFrame();

        glfwPollEvents(); // Poll for and process events

        auto ellapsedTime = glfwGetTime() - seconds;
        auto guiHasFocus = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
        if (!guiHasFocus) {
            // Put here code to handle user interactions
        }

		m_GLFWHandle.swapBuffers(); // Swap front and back buffers
    }

    return 0;
}

Application::Application(int argc, char** argv):
    m_AppPath { glmlv::fs::path{ argv[0] } },
    m_AppName { m_AppPath.stem().string() },
    m_ImGuiIniFilename { m_AppName + ".imgui.ini" },
    m_ShadersRootPath { m_AppPath.parent_path() / "shaders" }

{
    ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file

    // Put here initialization code
	const auto applicationPath = glmlv::fs::path{ argv[0] };
	const auto shadersRootPath = applicationPath.parent_path() / "shaders";

	auto program = glmlv::compileProgram(shadersRootPath + "forward.vs.glsl",
		shadersRootPath + "forward.fs.glsl");
	
	program.use();


	const auto Cube = glmlv::makeCube();
	count = Cube.vertexBuffer.size();
	//vao
	glGenBuffers(1, &vao);
	glGenVertexArrays(1, &vao);
	//vbo
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//ibo
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	glBufferStorage(GL_ARRAY_BUFFER, sizeof(Cube.vertexBuffer[0])*Cube.vertexBuffer.size(), Cube.vertexBuffer.data(), 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//Postion
	const auto positionAttrIdx = 0;
	glEnableVertexAttribArray(positionAttrIdx);
	glVertexAttribPointer(
		positionAttrIdx,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(glmlv::Vertex3f3f2f),
		(const GLvoid*) offsetof(glmlv::Vertex3f3f2f, position));

	//normal
	const auto normalAttrIdx = 1;
	glEnableVertexAttribArray(normalAttrIdx);
	glVertexAttribPointer(
		normalAttrIdx,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(glmlv::Vertex3f3f2f),
		(const GLvoid*)offsetof(glmlv::Vertex3f3f2f, normal));

	//texCoords
	const auto texCoordsAttrIdx = 2;
	glEnableVertexAttribArray(texCoordsAttrIdx);
	glVertexAttribPointer(
		texCoordsAttrIdx,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(glmlv::Vertex3f3f2f),
		(const GLvoid*)offsetof(glmlv::Vertex3f3f2f, texCoords));



	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

}