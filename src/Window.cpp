#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <map>
#include <utility>
#include <cmath>

#include <iostream>

#include "Camera.h" 
#include "Mesh.h"
#include "Perlin.h"
#include "Actions.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);



const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 1000;

Camera camera(glm::vec3(8.0f, 5.0f, 20.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

std::map<std::pair<int, int>, ChunkMesh*> activeChunks;
int renderDistance = 3;
float deltaTime = 0.0f;	
float lastFrame = 0.0f;


const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec4 aColor;\n" 
    "out vec4 vertexColor;\n"
    "uniform mat4 model;\n"      
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "   vertexColor = aColor;\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec4 vertexColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vertexColor;\n" 
    "}\n\0";

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    Camera cam;
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Voxel Engine", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    ChunkMesh chunk(0,0);
    chunk.populateChunk();
    chunk.buildMesh();


    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glEnable(GL_DEPTH_TEST);
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projLoc = glGetUniformLocation(shaderProgram, "projection");


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // render loop
    while (!glfwWindowShouldClose(window)) 
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        glUseProgram(shaderProgram);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
       
        int playerChunkX = static_cast<int>(std::floor(camera.Position.x / CHUNK_SIZE));
        int playerChunkZ = static_cast<int>(std::floor(camera.Position.z / CHUNK_SIZE));
        std::string title = "x: "+ (std::to_string(camera.Position.x)) + "  y: " + std::to_string(camera.Position.x);
        glfwSetWindowTitle(window, title.c_str());
         
        for (int x = playerChunkX - renderDistance; x <= playerChunkX + renderDistance; x++) {
            for (int z = playerChunkZ - renderDistance; z <= playerChunkZ + renderDistance; z++) {
                
                std::pair<int, int> chunkCoord(x, z);
                
                if (activeChunks.find(chunkCoord) == activeChunks.end()) {
                    
                    ChunkMesh* newChunk = new ChunkMesh(x, z);
                    newChunk->populateChunk();
                    newChunk->buildMesh();
                    newChunk->memory(); 
                    \
                    activeChunks[chunkCoord] = newChunk;
                }
            }
        }

        for (auto const& [coord, chunk] : activeChunks) {
            
            glBindVertexArray(chunk->VAO);
            
            glm::mat4 chunkModel = glm::mat4(1.0f); 
            chunkModel = glm::translate(chunkModel, glm::vec3(chunk->chunkX * CHUNK_SIZE, 0.0f, chunk->chunkZ * CHUNK_SIZE));
            
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(chunkModel));

            // Draw the chunk
            int vertexCount = chunk->meshVertices.size() / 3;
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
        
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if(glfwGetKey(window,GLFW_KEY_SPACE)==GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if(glfwGetKey(window,GLFW_KEY_LEFT_SHIFT)==GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS){
        
        Actions actions;
        RaycastResult ray = actions.getLookingAt(camera.Position, glm::normalize(camera.Front), 5.0f, activeChunks);
        if(ray.hit){
            actions.breakBlock(ray, activeChunks);
        }
        
        
    }
}


void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height){

}
