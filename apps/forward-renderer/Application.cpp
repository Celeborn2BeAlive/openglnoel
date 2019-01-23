#include "Application.hpp"

#include <iostream>

#include <glmlv/Image2DRGBA.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glmlv/GLProgram.hpp>

using namespace std;
using namespace glmlv;

int Application::run()
{


	//MATRICES

    /*---------------------------------------------------------------------------------------------*/
    /*-----------------------------------INITIALISATION DU CODE------------------------------------*/
    /*---------------------------------------------------------------------------------------------*/
        std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    /*--------------------------------------------*/
    float clearColor[3] = { 0.5, 0.8, 0.2 };
    glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);
    
    
    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
    
        //POUR FPS
        const auto seconds = glfwGetTime();

        const auto viewportSize = m_GLFWHandle.framebufferSize();
        glViewport(0, 0, viewportSize.x, viewportSize.y);

        const auto projMatrix = glm::perspective(glm::radians(70.f), float(viewportSize.x) / viewportSize.y, 0.01f, 100.f);
        const auto viewMatrix = m_viewController.getViewMatrix();
        const auto modelMatrix = glm::rotate(glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)), 0.2f * float(seconds), glm::vec3(0, 1, 0));

        const auto mvMatrix = viewMatrix * modelMatrix;
        const auto mvpMatrix = projMatrix * mvMatrix;
        const auto normalMatrix = glm::transpose(glm::inverse(mvMatrix));
        //POUR FPS
    
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //POUR LA LIGHT
        glUniform3fv(loc_DLD, 1, glm::value_ptr(glm::vec3(viewMatrix * glm::vec4(glm::normalize(m_DirLightDirection), 0))));
        glUniform3fv(loc_DLI, 1, glm::value_ptr(m_DirLightColor * m_DirLightIntensity));
        glUniform3fv(loc_PLP, 1, glm::value_ptr(glm::vec3(viewMatrix * glm::vec4(m_PointLightPosition, 1))));
        glUniform3fv(loc_PLI, 1, glm::value_ptr(m_PointLightColor * m_PointLightIntensity));
        //POUR LA LIGHT

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(loc_tex, 0);

        glUniformMatrix4fv(loc_MVP,1,GL_FALSE,glm::value_ptr(mvpMatrix));
        glUniformMatrix4fv(loc_MV,1,GL_FALSE,glm::value_ptr(mvMatrix));
        glUniformMatrix4fv(loc_N,1,GL_FALSE,glm::value_ptr(normalMatrix));

        glUniform3fv(loc_KD , 1, glm::value_ptr(m_SphereKd));

	//bind texture puis vao
	glBindTexture(GL_TEXTURE_2D, m_sphereTXO);
        glBindVertexArray(m_sphereVAO);

        glDrawElements(GL_TRIANGLES, m_sphereGeometry.indexBuffer.size(), GL_UNSIGNED_INT, nullptr);

	//debind vao puis texture
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);


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
                ImGui::ColorEdit3("Sphere Kd", glm::value_ptr(m_SphereKd));
            }

            ImGui::End();
        }
        
        glmlv::imguiRenderFrame();

        
        /* Poll for and process events */
        glfwPollEvents();
        
        // Update the display
	m_GLFWHandle.swapBuffers(); // Swap front and back buffers

        auto ellapsedTime = glfwGetTime() - seconds;
        auto guiHasFocus = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
        if (!guiHasFocus) {
            m_viewController.update(float(ellapsedTime));
        }

    }


    glDeleteBuffers(1,&m_sphereVBO);
    glDeleteBuffers(1,&m_sphereIBO);
    glDeleteVertexArrays(1,&m_sphereVAO);
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
    
        //PROGRAM
        program = glmlv::compileProgram({ m_ShadersRootPath / "forward-renderer" / "forward.vs.glsl", m_ShadersRootPath / "forward-renderer" /  "forward.fs.glsl" });
        program.use();

	//RECUPERATION VARIABLE UNIFORMES
        loc_MVP = program.getUniformLocation("uMVPMatrix"); 
        loc_MV = program.getUniformLocation("uMVMatrix");
        loc_N = program.getUniformLocation("uNormalMatrix");

        loc_tex = program.getUniformLocation("uKdSampler");

        loc_DLD = program.getUniformLocation("uDirectionalLightDir"); 
        loc_DLI = program.getUniformLocation("uDirectionalLightIntensity");
        loc_PLP = program.getUniformLocation("uPointLightPosition");
        loc_PLI = program.getUniformLocation("uPointLightIntensity");
        loc_KD = program.getUniformLocation("uKd");
    
        //VAR DE TYPE SPHERE
        m_sphereGeometry = makeSphere(60);
        

        //init, bind, buffdata et debind du m_sphereVBO
        glGenBuffers(1, &m_sphereVBO);
        glBindBuffer(GL_ARRAY_BUFFER,m_sphereVBO);
        glBufferData(GL_ARRAY_BUFFER,
        		m_sphereGeometry.vertexBuffer.size()*sizeof(Vertex3f3f2f),
        		m_sphereGeometry.vertexBuffer.data(),
        		GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER,0);


        //init, bind, buffdata et debind du m_sphereIBO
        glGenBuffers(1, &m_sphereIBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sphereIBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_sphereGeometry.indexBuffer.size() * sizeof(uint32_t), m_sphereGeometry.indexBuffer.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


        //init et bind du m_sphereVAO
        glGenVertexArrays(1,&m_sphereVAO);
        glBindVertexArray(m_sphereVAO);

        //rebind IBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sphereIBO);
        //rebind du m_sphereVBO
        glBindBuffer(GL_ARRAY_BUFFER,m_sphereVBO);

        //init et attrib array
        const GLuint VERTEX_ATTR_POSITION = 0;
        const GLuint VERTEX_ATTR_NORMAL = 1;
        const GLuint VERTEX_ATTR_TEXTURE = 2;
        glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
        glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
    	glEnableVertexAttribArray(VERTEX_ATTR_TEXTURE);

        //attrib pointer
        glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3f3f2f), 0);
        glVertexAttribPointer(VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3f3f2f), (const GLvoid*)offsetof(Vertex3f3f2f,normal));
        glVertexAttribPointer(VERTEX_ATTR_TEXTURE, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3f3f2f), (const GLvoid*)offsetof(Vertex3f3f2f, texCoords));

        //debin vbo vao 
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0);

       	//TEXTURE
	glActiveTexture(GL_TEXTURE0);
        glmlv::Image2DRGBA image = glmlv::readImage(m_AssetsRootPath / m_AppName / "textures" / "EarthMap.jpg");
        glGenTextures(1, &m_sphereTXO);
        	//Pour Terre
        glBindTexture(GL_TEXTURE_2D, m_sphereTXO);

  	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, image.width(), image.height());
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width(), image.height(), GL_RGBA, GL_UNSIGNED_BYTE, image.data());
 

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
   	//FIN TEXTURE


        //Pour la vue FPS
        m_viewController.setViewMatrix(glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));

        //activer test profondeur GPU
        glEnable(GL_DEPTH_TEST);

}
