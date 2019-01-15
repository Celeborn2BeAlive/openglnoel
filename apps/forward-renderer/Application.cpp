#include "Application.hpp"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glmlv/GLProgram.hpp>

using namespace glmlv;

int Application::run()
{


	//MATRICES

    /*---------------------------------------------------------------------------------------------*/
    /*-----------------------------------INITIALISATION DU CODE------------------------------------*/
    /*---------------------------------------------------------------------------------------------*/
        std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    
        const auto program = glmlv::compileProgram({ m_ShadersRootPath / "forward-renderer" / "forward.vs.glsl", m_ShadersRootPath / "forward-renderer" /  "forward.fs.glsl" });
        program.use();
        
        float ratio = m_nWindowWidth/m_nWindowHeight;

        //glm::mat4 ProjMatrix = glm::perspective(glm::radians(70.f),ratio,0.1f,100.f);
        //glm::mat4 MVMatrix = glm::translate(glm::mat4(1),glm::vec3(0.0f,0.0f,-5.0f));
        //glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        
        //LOCALISATION VAR UNIFORMES
        GLint loc_MVP = program.getUniformLocation("uMVPMatrix"); 
        GLint loc_MV = program.getUniformLocation("uMVMatrix");
        GLint loc_N = program.getUniformLocation("uNormalMatrix");
        
        
    /*--------------------------------------------*/
    
    
    
    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
    
        //POUR FPS
        const auto seconds = glfwGetTime();

        const auto viewportSize = m_GLFWHandle.framebufferSize();
        glViewport(0, 0, viewportSize.x, viewportSize.y);

        const auto projMatrix = glm::perspective(glm::radians(70.f), float(viewportSize.x) / viewportSize.y, 0.01f, 100.f);
        const auto viewMatrix = m_viewController.getViewMatrix();
        const auto modelMatrix = glm::rotate(glm::translate(glm::mat4(1), glm::vec3(-2, 0, 0)), 0.2f * float(seconds), glm::vec3(0, 1, 0));

        const auto mvMatrix = viewMatrix * modelMatrix;
        const auto mvpMatrix = projMatrix * mvMatrix;
        const auto normalMatrix = glm::transpose(glm::inverse(mvMatrix));
        //POUR FPS
    
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glBindVertexArray(m_sphereVAO);

        glUniformMatrix4fv(loc_MVP,1,GL_FALSE,glm::value_ptr(mvpMatrix));
        glUniformMatrix4fv(loc_MV,1,GL_FALSE,glm::value_ptr(mvMatrix));
        glUniformMatrix4fv(loc_N,1,GL_FALSE,glm::value_ptr(normalMatrix));

        glDrawElements(GL_TRIANGLES, m_sphereGeometry.indexBuffer.size(), GL_UNSIGNED_INT, nullptr);
  
        glBindVertexArray(0);


    
        
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
    m_ShadersRootPath { m_AppPath.parent_path() / "shaders" }

{
    ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file
    
    
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


        //m_sphereIBO
        GLuint m_sphereIBO;
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

        //debinder tout
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0);

        //Pour la vue FPS
        m_viewController.setViewMatrix(glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));

        //activer test profondeur GPU
        glEnable(GL_DEPTH_TEST);

}
