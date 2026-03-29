#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const char *uiVertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n" 
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n" 
    "}\0";

const char *uiFragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 1.0f, 1.0f, 0.8f);\n" 
    "}\n\0";

class UI {
public:
    unsigned int crosshairVAO, crosshairVBO;
    unsigned int uiShaderProgram; 

    void set2D() {
        unsigned int uiVertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(uiVertexShader, 1, &uiVertexShaderSource, NULL);
        glCompileShader(uiVertexShader);

        unsigned int uiFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(uiFragmentShader, 1, &uiFragmentShaderSource, NULL);
        glCompileShader(uiFragmentShader);

        uiShaderProgram = glCreateProgram(); // Create it here!
        glAttachShader(uiShaderProgram, uiVertexShader);
        glAttachShader(uiShaderProgram, uiFragmentShader);
        glLinkProgram(uiShaderProgram);

        glDeleteShader(uiVertexShader);
        glDeleteShader(uiFragmentShader);

        float crosshairVertices[] = {
            // Horizontal line
            -0.02f,  0.0f,   
             0.02f,  0.0f,   
            // Vertical line
             0.0f,  -0.035f, 
             0.0f,   0.035f  
        };

        glGenVertexArrays(1, &crosshairVAO);
        glGenBuffers(1, &crosshairVBO);

        glBindVertexArray(crosshairVAO);
        glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glBindVertexArray(0);
    }
};