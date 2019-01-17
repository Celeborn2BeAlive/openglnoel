#include "Application.hpp"

#include <iostream>

#include <glmlv/Image2DRGBA.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>


int Application::run()
{

	float clearColor[3] = { 0.5, 0.8, 0.2 };
	glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);
	//glBindVertexArray(vaoCube);
	//glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);

    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        // Put here rendering code
		const auto viewportSize = m_GLFWHandle.framebufferSize();
		glViewport(0, 0, viewportSize.x, viewportSize.y);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		const auto projMatrix = glm::perspective(70.f, float(viewportSize.x) / viewportSize.y, 0.01f, 100.f);
		const auto viewMatrix = viewController.getViewMatrix();

		//light 
		glUniform3fv(uDirectionalLightDirLocation, 1, glm::value_ptr(glm::vec3(viewMatrix * glm::vec4(glm::normalize(DirLightDirection), 0))));
		glUniform3fv(uDirectionalLightIntensityLocation, 1, glm::value_ptr(DirLightColor * DirLightIntensity));

		glUniform3fv(uPointLightPositionLocation, 1, glm::value_ptr(glm::vec3(viewMatrix * glm::vec4(PointLightPosition, 1))));
		glUniform3fv(uPointLightIntensityLocation, 1, glm::value_ptr(PointLightColor * m_PointLightIntensity));

		//texture
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(uKdSamplerLocation, 0); 
		glBindSampler(0, textureSampler); 

		//for the cube
		{
			const auto modelMatrix = glm::rotate(glm::translate(glm::mat4(1), glm::vec3(-2, 0, 0)), 0.2f * float(seconds), glm::vec3(0, 1, 0));

			const auto mvMatrix = viewMatrix * modelMatrix;
			const auto mvpMatrix = projMatrix * mvMatrix;
			const auto normalMatrix = glm::transpose(glm::inverse(mvMatrix));

			glUniformMatrix4fv(uModelViewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
			glUniformMatrix4fv(uModelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
			glUniformMatrix4fv(uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

			glUniform3fv(uKdLocation, 1, glm::value_ptr(CubeKd));

			glBindTexture(GL_TEXTURE_2D, cubeTextureKd);

			glBindVertexArray(vaoCube);
			glDrawElements(GL_TRIANGLES, Cube.indexBuffer.size(), GL_UNSIGNED_INT, nullptr);
		}

		//for the sphere
		{
			const auto modelMatrix = glm::rotate(glm::translate(glm::mat4(1), glm::vec3(2, 0, 0)), 0.2f * float(seconds), glm::vec3(0, 1, 0));

			const auto mvMatrix = viewMatrix * modelMatrix;
			const auto mvpMatrix = projMatrix * mvMatrix;
			const auto normalMatrix = glm::transpose(glm::inverse(mvMatrix));

			glUniformMatrix4fv(uModelViewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
			glUniformMatrix4fv(uModelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
			glUniformMatrix4fv(uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

			glUniform3fv(uKdLocation, 1, glm::value_ptr(SphereKd));

			glBindTexture(GL_TEXTURE_2D, sphereTextureKd);

			glBindVertexArray(vaoSphere);
			glDrawElements(GL_TRIANGLES, Sphere.indexBuffer.size(), GL_UNSIGNED_INT, nullptr);
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindSampler(0, 0); // Unbind the sampler


        // GUI code:
		glmlv::imguiNewFrame();

        {
            ImGui::Begin("GUI");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			if (ImGui::ColorEdit3("clearColor", clearColor)) {
				glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);
			}
			//light
			if (ImGui::CollapsingHeader("Directional Light"))
			{
				ImGui::ColorEdit3("DirLightColor", glm::value_ptr(DirLightColor));
				ImGui::DragFloat("DirLightIntensity", &DirLightIntensity, 0.1f, 0.f, 100.f);
				if (ImGui::DragFloat("Phi Angle", &DirLightPhiAngleDegrees, 1.0f, 0.0f, 360.f) ||
					ImGui::DragFloat("Theta Angle", &DirLightThetaAngleDegrees, 1.0f, 0.0f, 180.f)) {
					DirLightDirection = computeDirectionVector(glm::radians(DirLightPhiAngleDegrees), glm::radians(DirLightThetaAngleDegrees));
				}
			}

			if (ImGui::CollapsingHeader("Point Light"))
			{
				ImGui::ColorEdit3("PointLightColor", glm::value_ptr(PointLightColor));
				ImGui::DragFloat("PointLightIntensity", &m_PointLightIntensity, 0.1f, 0.f, 16000.f);
				ImGui::InputFloat3("Position", glm::value_ptr(PointLightPosition));
			}

			if (ImGui::CollapsingHeader("Materials"))
			{
				ImGui::ColorEdit3("Cube Kd", glm::value_ptr(CubeKd));
				ImGui::ColorEdit3("Sphere Kd", glm::value_ptr(SphereKd));
			}

            ImGui::End();
        }

		glmlv::imguiRenderFrame();

        glfwPollEvents(); // Poll for and process events

        auto ellapsedTime = glfwGetTime() - seconds;
        auto guiHasFocus = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
        if (!guiHasFocus) {
            // Put here code to handle user interactions
			viewController.update(float(ellapsedTime));
        }

		m_GLFWHandle.swapBuffers(); // Swap front and back buffers
    }

    return 0;
}

Application::Application(int argc, char** argv):
    m_AppPath { glmlv::fs::path{ argv[0] } },
    m_AppName { m_AppPath.stem().string() },
    m_ImGuiIniFilename { m_AppName + ".imgui.ini" },
    m_ShadersRootPath { m_AppPath.parent_path() / "shaders" },
	m_AssetsRootPath{ m_AppPath.parent_path() / "assets" }

{


	ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file

	const GLint vboBindingIndex = 0; // Arbitrary choice between 0 and glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS)

	const GLint positionAttrLocation = 0;
	const GLint normalAttrLocation = 1;
	const GLint texCoordsAttrLocation = 2;

	glGenBuffers(1, &vboCube);
	glGenBuffers(1, &iboCube);
	glGenBuffers(1, &vboSphere);
	glGenBuffers(1, &iboSphere);

	Cube = glmlv::makeCube();
	Sphere = glmlv::makeSphere(32);

	glBindBuffer(GL_ARRAY_BUFFER, vboCube);
	glBufferStorage(GL_ARRAY_BUFFER, Cube.vertexBuffer.size() * sizeof(glmlv::Vertex3f3f2f), Cube.vertexBuffer.data(), 0);

	glBindBuffer(GL_ARRAY_BUFFER, vboSphere);
	glBufferStorage(GL_ARRAY_BUFFER, Sphere.vertexBuffer.size() * sizeof(glmlv::Vertex3f3f2f), Sphere.vertexBuffer.data(), 0);

	glBindBuffer(GL_ARRAY_BUFFER, iboCube);
	glBufferStorage(GL_ARRAY_BUFFER, Cube.indexBuffer.size() * sizeof(uint32_t), Cube.indexBuffer.data(), 0);

	glBindBuffer(GL_ARRAY_BUFFER, iboSphere);
	glBufferStorage(GL_ARRAY_BUFFER, Sphere.indexBuffer.size() * sizeof(uint32_t), Sphere.indexBuffer.data(), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Lets use a lambda to factorize VAO initialization:
	const auto initVAO = [positionAttrLocation, normalAttrLocation, texCoordsAttrLocation](GLuint& vao, GLuint vbo, GLuint ibo)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// We tell OpenGL what vertex attributes our VAO is describing:
		glEnableVertexAttribArray(positionAttrLocation);
		glEnableVertexAttribArray(normalAttrLocation);
		glEnableVertexAttribArray(texCoordsAttrLocation);

		glBindBuffer(GL_ARRAY_BUFFER, vbo); // We bind the VBO because the next 3 calls will read what VBO is bound in order to know where the data is stored

		glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*)offsetof(glmlv::Vertex3f3f2f, position));
		glVertexAttribPointer(normalAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*)offsetof(glmlv::Vertex3f3f2f, normal));
		glVertexAttribPointer(texCoordsAttrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*)offsetof(glmlv::Vertex3f3f2f, texCoords));

		glBindBuffer(GL_ARRAY_BUFFER, 0); // We can unbind the VBO because OpenGL has "written" in the VAO what VBO it needs to read when the VAO will be drawn

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); // Binding the IBO to GL_ELEMENT_ARRAY_BUFFER while a VAO is bound "writes" it in the VAO for usage when the VAO will be drawn

		glBindVertexArray(0);
	};

	initVAO(vaoCube, vboCube, iboCube);
	initVAO(vaoSphere, vboSphere, iboSphere);

	//texture
	glActiveTexture(GL_TEXTURE0); // We will work on GL_TEXTURE0 texture unit. Since the shader only use one texture at a time, we only need one texture unit
	{
		glmlv::Image2DRGBA image = glmlv::readImage(m_AssetsRootPath / m_AppName / "textures" / "img1.jpg");

		glGenTextures(1, &cubeTextureKd);
		glBindTexture(GL_TEXTURE_2D, cubeTextureKd);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, image.width(), image.height());
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width(), image.height(), GL_RGBA, GL_UNSIGNED_BYTE, image.data());
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	{
		glmlv::Image2DRGBA image = glmlv::readImage(m_AssetsRootPath / m_AppName / "textures" / "img2.png");

		glGenTextures(1, &sphereTextureKd);
		glBindTexture(GL_TEXTURE_2D, sphereTextureKd);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, image.width(), image.height());
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width(), image.height(), GL_RGBA, GL_UNSIGNED_BYTE, image.data());
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Note: no need to bind a sampler for modifying it: the sampler API is already direct_state_access
	glGenSamplers(1, &textureSampler);
	glSamplerParameteri(textureSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(textureSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);




	glEnable(GL_DEPTH_TEST);

	program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "forward.vs.glsl", m_ShadersRootPath / m_AppName / "forward.fs.glsl" });
	program.use();

	
	//light
	uModelViewProjMatrixLocation = glGetUniformLocation(program.glId(), "uModelViewProjMatrix");
	uModelViewMatrixLocation = glGetUniformLocation(program.glId(), "uModelViewMatrix");
	uNormalMatrixLocation = glGetUniformLocation(program.glId(), "uNormalMatrix");

	uModelViewProjMatrixLocation = glGetUniformLocation(program.glId(), "uModelViewProjMatrix");
	uModelViewMatrixLocation = glGetUniformLocation(program.glId(), "uModelViewMatrix");
	uNormalMatrixLocation = glGetUniformLocation(program.glId(), "uNormalMatrix");


	uDirectionalLightDirLocation = glGetUniformLocation(program.glId(), "uDirectionalLightDir");
	uDirectionalLightIntensityLocation = glGetUniformLocation(program.glId(), "uDirectionalLightIntensity");

	uPointLightPositionLocation = glGetUniformLocation(program.glId(), "uPointLightPosition");
	uPointLightIntensityLocation = glGetUniformLocation(program.glId(), "uPointLightIntensity");

	uKdLocation = glGetUniformLocation(program.glId(), "uKd");

	uKdSamplerLocation = glGetUniformLocation(program.glId(), "uKdSampler");

	//view
	viewController.setViewMatrix(glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));

}

//OLD CODE
/*
ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file

	// Put here initialization code

	auto program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "forward.vs.glsl",
		m_ShadersRootPath / m_AppName / "forward.fs.glsl" });

	program.use();


	Cube = glmlv::makeCube();
	Sphere = glmlv::makeSphere(32);

	count = Cube.vertexBuffer.size();
	//vao cube
	glGenBuffers(1, &vaoCube);
	glGenVertexArrays(1, &vaoCube);

	//vao sphere
	glGenBuffers(1, &vaoSphere);
	glGenVertexArrays(1, &vaoSphere);

	//vbo
	glGenBuffers(1, &vboCube);
	glBindBuffer(GL_ARRAY_BUFFER, vboCube);
	//ibo
	glGenBuffers(1, &iboCube);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboCube);

	glBufferStorage(GL_ARRAY_BUFFER, sizeof(Cube.vertexBuffer[0])*Cube.vertexBuffer.size(), Cube.vertexBuffer.data(), 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenVertexArrays(1, &vaoCube);
	glBindVertexArray(vaoCube);
	glBindBuffer(GL_ARRAY_BUFFER, vboCube);

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

	m_uModelViewProjMatrixLocation = glGetUniformLocation(program.glId(), "uModelViewProjMatrix");
	m_uModelViewMatrixLocation = glGetUniformLocation(program.glId(), "uModelViewMatrix");
	m_uNormalMatrixLocation = glGetUniformLocation(program.glId(), "uNormalMatrix");
*/