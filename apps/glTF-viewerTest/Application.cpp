#include "Application.hpp"
#include "trackball.hpp"

#include <iostream>

#include <glmlv/Image2DRGBA.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>

//include to use callback functions for camera controls
#include <GLFW/glfw3.h>

#ifndef TINYGLTF_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>
#endif

int Application::run()
{
	float clearColor[3] = { 0.5, 0.8, 0.2 };
	glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);

    // Loop until the user closes the windows
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        // Put here rendering code
        const auto viewportSize = m_GLFWHandle.framebufferSize();
        glViewport(0, 0, viewportSize.x, viewportSize.y);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // WORLD MATRIX
        const auto projMatrix = glm::perspective(70.f, float(viewportSize.x) / viewportSize.y, 0.01f, 100.f);
        const auto viewMatrix = m_viewController.getViewMatrix();

        // LIGHT
        glUniform3fv(m_uDirectionalLightDirLocation, 1, glm::value_ptr(glm::vec3(viewMatrix * glm::vec4(glm::normalize(m_DirLightDirection), 0))));
        glUniform3fv(m_uDirectionalLightIntensityLocation, 1, glm::value_ptr(m_DirLightColor * m_DirLightIntensity));

        glUniform3fv(m_uPointLightPositionLocation, 1, glm::value_ptr(glm::vec3(viewMatrix * glm::vec4(m_PointLightPosition, 1))));
        glUniform3fv(m_uPointLightIntensityLocation, 1, glm::value_ptr(m_PointLightColor * m_PointLightIntensity));

        // ACTIVE TEXTURE
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(m_uKdSamplerLocation, 0); // Set the uniform to 0 because we use texture unit 0
        glBindSampler(0, m_textureSampler); // Tell to OpenGL what sampler we want to use on this texture unit


		/*
		//window
		window = glfwCreateWindow(m_nWindowWidth, m_nWindowHeight, "Best glTF viewer", NULL, NULL);
		if (window == NULL) {
			std::cerr << "Failed to open GLFW window. " << std::endl;
			glfwTerminate();
			return 1;
		}

		glfwGetWindowSize(window, &m_nWindowWidth, &m_nWindowHeight);

		glfwMakeContextCurrent(window);
		*/

		//camera
		glfwSetKeyCallback(m_GLFWHandle.window(), keyboardFunc);
		glfwSetMouseButtonCallback(m_GLFWHandle.window(), clickFunc);
		glfwSetCursorPosCallback(m_GLFWHandle.window(), motionFunc);

        // GLTF DRAWING
        {
            // OBJECT MATRIX
            const auto modelMatrix = glm::mat4(1);

            const auto mvMatrix = viewMatrix * modelMatrix;
            const auto mvpMatrix = projMatrix * mvMatrix;
            const auto normalMatrix = glm::transpose(glm::inverse(mvMatrix));

            glUniformMatrix4fv(m_uModelViewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
            glUniformMatrix4fv(m_uModelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
            glUniformMatrix4fv(m_uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

            // DRAW GLTF
            for (size_t i = 0; i < m_vaos.size(); ++i)
            {        
                const tinygltf::Accessor &indexAccessor = m_model.accessors[m_primitives[i].indices];        
                glBindVertexArray(m_vaos[i]);
                glDrawElements(getMode(m_primitives[i].mode), indexAccessor.count, indexAccessor.componentType, (const GLvoid*) indexAccessor.byteOffset);
            }
        }

        // UNBIND
        glBindVertexArray(0);
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
            if (ImGui::CollapsingHeader("Directional Light"))
            {
                ImGui::ColorEdit3("DirLightColor", glm::value_ptr(m_DirLightColor));
                ImGui::DragFloat("DirLightIntensity", &m_DirLightIntensity, 0.1f, 0.f, 100.f);
                if (ImGui::DragFloat("Phi Angle", &m_DirLightPhiAngleDegrees, 1.0f, 0.0f, 360.f) ||
                    ImGui::DragFloat("Theta Angle", &m_DirLightThetaAngleDegrees, 1.0f, 0.0f, 180.f)) {
                    m_DirLightDirection = computeDirectionVector(glm::radians(m_DirLightPhiAngleDegrees), glm::radians(m_DirLightThetaAngleDegrees));
                }
            }

            if (ImGui::CollapsingHeader("Point Light"))
            {
                ImGui::ColorEdit3("PointLightColor", glm::value_ptr(m_PointLightColor));
                ImGui::DragFloat("PointLightIntensity", &m_PointLightIntensity, 0.1f, 0.f, 16000.f);
                ImGui::InputFloat3("Position", glm::value_ptr(m_PointLightPosition));
            }

            if (ImGui::CollapsingHeader("Materials"))
            {
                ImGui::ColorEdit3("Cube Kd", glm::value_ptr(m_CubeKd));
                ImGui::ColorEdit3("Sphere Kd", glm::value_ptr(m_SphereKd));
            }

            ImGui::End();
        }

		glmlv::imguiRenderFrame();

        /* Poll for and process events */
        glfwPollEvents();

        /* Swap front and back buffers*/
        m_GLFWHandle.swapBuffers();

        auto ellapsedTime = glfwGetTime() - seconds;
        auto guiHasFocus = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
        if (!guiHasFocus) {
            m_viewController.update(float(ellapsedTime));
        }
    }

    return 0;
}

Application::Application(int argc, char** argv):
    m_AppPath { glmlv::fs::path{ argv[0] } },
    m_AppName { m_AppPath.stem().string() },
    m_ImGuiIniFilename { m_AppName + ".imgui.ini" },
    m_ShadersRootPath { m_AppPath.parent_path() / "shaders" },
    m_AssetsRootPath { m_AppPath.parent_path() / "assets" }

{
    ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file

    // 1 - LINK WITH SHADERS

    // Attribute location
    const GLint positionAttrLocation = 0;
    const GLint normalAttrLocation = 1;
    const GLint texCoordsAttrLocation = 2;
    
    // Map gltf attributes name to an index value of vs
    m_attribs["POSITION"] = positionAttrLocation;
    m_attribs["NORMAL"] = normalAttrLocation;
    m_attribs["TEXCOORD_0"] = texCoordsAttrLocation;

    glEnable(GL_DEPTH_TEST);

    // Compile Shaders
    m_program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "forward.vs.glsl", m_ShadersRootPath / m_AppName / "forward.fs.glsl" });
    m_program.use();

    // Get Uniforms
    m_uModelViewProjMatrixLocation = glGetUniformLocation(m_program.glId(), "uModelViewProjMatrix");
    m_uModelViewMatrixLocation = glGetUniformLocation(m_program.glId(), "uModelViewMatrix");
    m_uNormalMatrixLocation = glGetUniformLocation(m_program.glId(), "uNormalMatrix");

    m_uDirectionalLightDirLocation = glGetUniformLocation(m_program.glId(), "uDirectionalLightDir");
    m_uDirectionalLightIntensityLocation = glGetUniformLocation(m_program.glId(), "uDirectionalLightIntensity");

    m_uPointLightPositionLocation = glGetUniformLocation(m_program.glId(), "uPointLightPosition");
    m_uPointLightIntensityLocation = glGetUniformLocation(m_program.glId(), "uPointLightIntensity");

    m_uKdLocation = glGetUniformLocation(m_program.glId(), "uKd");
    m_uKdSamplerLocation = glGetUniformLocation(m_program.glId(), "uKdSampler");

    // 2 - SET CAMERA POSTION
    m_viewController.setViewMatrix(glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));

    // 3 - LOAD SCENE (.GLTF)
    if (argc < 2) {
        printf("Needs input.gltf\n");
        exit(1);
    }
    const glmlv::fs::path gltfPath = m_AssetsRootPath / m_AppName / glmlv::fs::path{ argv[1] };

    loadTinyGLTF(gltfPath);
}


// Load an obj m_model with tinyGLTF
void Application::loadTinyGLTF(const glmlv::fs::path & gltfPath)
{
    // 1 - LOAD
    //tinygltf::Model m_model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool ret = false;
    std::cout << "Reading ASCII glTF" << std::endl;
    // assume ascii glTF.
	std::cout << gltfPath.string() << std::endl;
    ret = loader.LoadASCIIFromFile(&m_model, &err, &warn, gltfPath.string());

    // Catch errors
    if (!warn.empty()) {
        printf("Warn: %s\n", warn.c_str());
    }

    if (!err.empty()) {
        printf("Err: %s\n", err.c_str());
    }

    if (!ret) {
        printf("Failed to parse glTF\n");
    }

    // 2 - BUFFERS (VBO / IBO)
    
    std::vector<GLuint> buffers(m_model.buffers.size()); // un par tinygltf::Buffer
    //m_buffers.resize(m_model.buffers.size());
    glGenBuffers(buffers.size(), buffers.data());
    for (size_t i = 0; i < buffers.size(); ++i)
    {
        const tinygltf::BufferView &bufferView = m_model.bufferViews[i];
        glBindBuffer(bufferView.target, buffers[i]);
        glBufferStorage(bufferView.target, m_model.buffers[i].data.size(), m_model.buffers[i].data.data(), 0);
        glBindBuffer(bufferView.target, 0);
    }

    std::cout << "# of buffers : " << buffers.size() << std::endl;
    
    // 3 - VAOs & Primitives
    // Pour chaque VAO on va aussi stocker les données de la primitive associé car on doit l'utiliser lors du rendu

    // Pour chaque mesh
    for (size_t i = 0; i < m_model.meshes.size(); i++)
    {
        const tinygltf::Mesh &mesh = m_model.meshes[i];

        // Pour chaque primitives du mesh
        for (size_t primId = 0; primId < mesh.primitives.size(); primId++)				
        {
            const tinygltf::Primitive &primitive = mesh.primitives[primId];

            // 3 BIS - CREATE VAO FOR EACH PRIMITIVE
            // Generate VAO
            GLuint vaoId;
            glGenVertexArrays(1, &vaoId);
            glBindVertexArray(vaoId);

            // INDICES (IBO)
            tinygltf::Accessor &indexAccessor = m_model.accessors[primitive.indices];
            tinygltf::BufferView &bufferView = m_model.bufferViews[indexAccessor.bufferView];
            int &bufferIndex = bufferView.buffer;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[bufferIndex]); // Binding IBO
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexAccessor.bufferView); // Binding IBO

            // ATTRIBUTS
            // Pour chaque attributes de la primitive
            std::map<std::string, int>::const_iterator it(primitive.attributes.begin());
            std::map<std::string, int>::const_iterator itEnd(primitive.attributes.end());

            for (; it != itEnd; it++)
            {
                assert(it->second >= 0);
                tinygltf::Accessor &accessor = m_model.accessors[it->second];
                bufferView = m_model.bufferViews[accessor.bufferView];
                bufferIndex = bufferView.buffer;
                glBindBuffer(GL_ARRAY_BUFFER, buffers[bufferIndex]);    // Binding VBO
                //glBindBuffer(GL_ARRAY_BUFFER, accessor.bufferView);    // Binding VBO
                
                // Get number of component
                int size = 1;
                if (accessor.type == TINYGLTF_TYPE_SCALAR) {
                    size = 1;
                } else if (accessor.type == TINYGLTF_TYPE_VEC2) {
                    size = 2;
                } else if (accessor.type == TINYGLTF_TYPE_VEC3) {
                    size = 3;
                } else if (accessor.type == TINYGLTF_TYPE_VEC4) {
                    size = 4;
                } else {
                    assert(0);
                }
                
                // it->first would be "POSITION", "NORMAL", "TEXCOORD_0", ...
                if ((it->first.compare("POSITION") == 0) ||
                    (it->first.compare("NORMAL") == 0) ||
                    (it->first.compare("TEXCOORD_0") == 0))
                {
                    if (m_attribs[it->first] >= 0)
                    {
                        // Compute byteStride from Accessor + BufferView combination.
                        int byteStride =  accessor.ByteStride(m_model.bufferViews[accessor.bufferView]);
                        assert(byteStride != -1);
                        glEnableVertexAttribArray(m_attribs[it->first]);
                        glVertexAttribPointer(m_attribs[it->first], size, accessor.componentType, accessor.normalized ? GL_TRUE : GL_FALSE, byteStride, (const GLvoid*) (bufferView.byteOffset + accessor.byteOffset));
                    }
                }

                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }

            // On rempli le vao et les primitives
            m_vaos.push_back(vaoId);
            m_primitives.push_back(primitive);

            glBindVertexArray(0);
        }
    }
}

GLenum Application::getMode(int mode)
{
    if (mode == TINYGLTF_MODE_TRIANGLES) {
        return GL_TRIANGLES;
    } else if (mode == TINYGLTF_MODE_TRIANGLE_STRIP) {
        return GL_TRIANGLE_STRIP;
    } else if (mode == TINYGLTF_MODE_TRIANGLE_FAN) {
        return GL_TRIANGLE_FAN;
    } else if (mode == TINYGLTF_MODE_POINTS) {
        return GL_POINTS;
    } else if (mode == TINYGLTF_MODE_LINE) {
        return GL_LINES;
    } else if (mode == TINYGLTF_MODE_LINE_LOOP) {
        return GL_LINE_LOOP;
    } else {
        assert(0);
    }
}


void keyboardFunc(GLFWwindow *window, int key, int scancode, int action,
	int mods) {
	(void)scancode;
	(void)mods;
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		// Close window
		if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}
}

void clickFunc(GLFWwindow *window, int button, int action, int mods) {
	double x, y;
	glfwGetCursorPos(window, &x, &y);

	bool shiftPressed = (mods & GLFW_MOD_SHIFT);
	bool ctrlPressed = (mods & GLFW_MOD_CONTROL);

	if ((button == GLFW_MOUSE_BUTTON_LEFT) && (!shiftPressed) && (!ctrlPressed)) {
		mouseLeftPressed = true;
		mouseMiddlePressed = false;
		mouseRightPressed = false;
		if (action == GLFW_PRESS) {
			int id = -1;
			// int id = ui.Proc(x, y);
			if (id < 0) {  // outside of UI
				trackball(prev_quat, 0.0, 0.0, 0.0, 0.0);
			}
		}
		else if (action == GLFW_RELEASE) {
			mouseLeftPressed = false;
		}
	}
	if ((button == GLFW_MOUSE_BUTTON_RIGHT) ||
		((button == GLFW_MOUSE_BUTTON_LEFT) && ctrlPressed)) {
		if (action == GLFW_PRESS) {
			mouseRightPressed = true;
			mouseLeftPressed = false;
			mouseMiddlePressed = false;
		}
		else if (action == GLFW_RELEASE) {
			mouseRightPressed = false;
		}
	}
	if ((button == GLFW_MOUSE_BUTTON_MIDDLE) ||
		((button == GLFW_MOUSE_BUTTON_LEFT) && shiftPressed)) {
		if (action == GLFW_PRESS) {
			mouseMiddlePressed = true;
			mouseLeftPressed = false;
			mouseRightPressed = false;
		}
		else if (action == GLFW_RELEASE) {
			mouseMiddlePressed = false;
		}
	}
}

void motionFunc(GLFWwindow *window, double mouse_x, double mouse_y) {
	(void)window;
	float rotScale = 1.0f;
	float transScale = 2.0f;

	if (mouseLeftPressed) {
		trackball(prev_quat, rotScale * (2.0f * prevMouseX - m_nWindowWidth) / (float)m_nWindowWidth,
			rotScale * (m_nWindowHeight - 2.0f * prevMouseY) / (float)m_nWindowHeight,
			rotScale * (2.0f * mouse_x - m_nWindowWidth) / (float)m_nWindowWidth,
			rotScale * (m_nWindowHeight - 2.0f * mouse_y) / (float)m_nWindowHeight);

		add_quats(prev_quat, curr_quat, curr_quat);
	}
	else if (mouseMiddlePressed) {
		eye[0] += -transScale * (mouse_x - prevMouseX) / (float)m_nWindowWidth;
		lookat[0] += -transScale * (mouse_x - prevMouseX) / (float)m_nWindowWidth;
		eye[1] += transScale * (mouse_y - prevMouseY) / (float)m_nWindowHeight;
		lookat[1] += transScale * (mouse_y - prevMouseY) / (float)m_nWindowHeight;
	}
	else if (mouseRightPressed) {
		eye[2] += transScale * (mouse_y - prevMouseY) / (float)m_nWindowHeight;
		lookat[2] += transScale * (mouse_y - prevMouseY) / (float)m_nWindowHeight;
	}

	// Update mouse point
	prevMouseX = mouse_x;
	prevMouseY = mouse_y;
}

static void Init() {
	trackball(curr_quat, 0, 0, 0, 0);

	eye[0] = 0.0f;
	eye[1] = 0.0f;
	eye[2] = 0.3f;

	lookat[0] = 0.0f;
	lookat[1] = 0.0f;
	lookat[2] = 0.0f;

	up[0] = 0.0f;
	up[1] = 1.0f;
	up[2] = 0.0f;
}