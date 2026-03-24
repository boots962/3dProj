#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <map>
#include <utility>
#include <cmath>

#include <iostream>

#include "Camera.h" 
#include "Mesh.h"
#include "Perlin.h"
#include "Actions.h"
#include "UI.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);



const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 1000;

Camera camera(glm::vec3(8.0f, 150.0f, 20.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
    

std::map<std::pair<int, int>, ChunkMesh*> activeChunks;
int renderDistance = 3;
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

    Actions actions;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "out vec3 FragPos;\n" 
    "uniform mat4 model;\n"      
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   FragPos = vec3(model * vec4(aPos, 1.0));\n" 
    "   gl_Position = projection * view * vec4(FragPos, 1.0);\n"
    "   TexCoord = aTexCoord;\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "in vec3 FragPos;\n" 
    "uniform sampler2D textureAtlas;\n"
    "uniform vec3 viewPos;\n"  
    "uniform vec3 skyColor;\n" 
    "uniform vec3 sunColor;\n"
    "uniform bool isSun;\n" 
    "void main()\n"
    "{\n"
    "   if(isSun) {\n"
    "       FragColor = vec4(sunColor, 1.0);\n"
    "       return;\n"
    "   }\n"
    
    "   vec4 texColor = texture(textureAtlas, TexCoord);\n"
    "   if(texColor.a < 0.1) discard;\n"
    "   float dist = distance(viewPos, FragPos);\n"
    "   float fogFactor = clamp((dist - 40.0) / (90.0 - 40.0), 0.0, 1.0);\n"
    "   vec4 finalColor = mix(texColor, vec4(skyColor, 1.0), fogFactor);\n"
    "   FragColor = finalColor;\n"
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

    float vertices[] = {
        5.0f, 5.0f, 0.0f,
        5.0f, -5.0f, 0.0f,

        -5.0f, -5.0f, 0.0f,
        -5.0f, 5.0f, 0.0f
    };
    unsigned int indices[] = {
        0,1,2,
        0,2,3
    };
    unsigned int sunVAO;
    unsigned int sunEBO;
    unsigned int sunVBO;
    glGenVertexArrays(1, &sunVAO);  
    glBindVertexArray(sunVAO);
    
    glGenBuffers(1, &sunVBO);  
    glBindBuffer(GL_ARRAY_BUFFER, sunVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glGenBuffers(1, &sunEBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sunEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); //sun/moon

    glBindVertexArray(0);

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
    glm::vec3 skyColor(0.5f, 0.8f, 1.0f);

    
    int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
    int skyColorLoc = glGetUniformLocation(shaderProgram, "skyColor");

    
    glClearColor(skyColor.x, skyColor.y, skyColor.z, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

   

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    unsigned int textureAtlas;
    glGenTextures(1, &textureAtlas);
    glBindTexture(GL_TEXTURE_2D, textureAtlas);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); 
    unsigned char *data = stbi_load("image.jpg", &width, &height, &nrChannels, 4);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);
    UI ui;
    ui.set2D();
    
    
    int isSunLoc = glGetUniformLocation(shaderProgram, "isSun");
    int sunColorLoc = glGetUniformLocation(shaderProgram, "sunColor");

    while (!glfwWindowShouldClose(window)) 
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        if (deltaTime > 0.05f) deltaTime = 0.05f; 
        lastFrame = currentFrame;

        processInput(window);
        
        int playerChunkX = static_cast<int>(std::floor(camera.Position.x / CHUNK_WIDTH));
        int playerChunkZ = static_cast<int>(std::floor(camera.Position.z / CHUNK_WIDTH));
        std::string title = "x: "+ (std::to_string(camera.Position.x)) + "  y: " + std::to_string(camera.Position.y) + " z:" + std::to_string(camera.Position.z);
        glfwSetWindowTitle(window, title.c_str());

        for (int x = playerChunkX - renderDistance; x <= playerChunkX + renderDistance; x++) {
            for (int z = playerChunkZ - renderDistance; z <= playerChunkZ + renderDistance; z++) {
                std::pair<int, int> chunkCoord(x, z);
                if (activeChunks.find(chunkCoord) == activeChunks.end()) {
                    ChunkMesh* newChunk = new ChunkMesh(x, z);
                    newChunk->populateChunk();
                    activeChunks[chunkCoord] = newChunk;
                }
            }
        }

        for (int x = playerChunkX - renderDistance; x <= playerChunkX + renderDistance; x++) {
            for (int z = playerChunkZ - renderDistance; z <= playerChunkZ + renderDistance; z++) {
                std::pair<int, int> chunkCoord(x, z);
                ChunkMesh* chunk = activeChunks[chunkCoord];
                if (chunk->meshVertices.empty()) {
                    chunk->buildMesh(activeChunks); 
                    chunk->memory(); 
                }
            }
        }

        glClearColor(skyColor.x, skyColor.y, skyColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        // Get matrices ready
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 200.0f);
        
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glDisable(GL_DEPTH_TEST); // Draw behind everything
        glUniform1i(isSunLoc, 1); 

        glm::mat4 sunModel = glm::mat4(1.0f);
        float orbit = 0.01f; // Slowed it down slightly so it's less chaotic
        sunModel = glm::rotate(sunModel, (float)glfwGetTime() * orbit, glm::vec3(1.0f, 0.0f, 0.0f));
        sunModel = glm::translate(sunModel, glm::vec3(0.0f, 0.0f, -50.0f));

        glm::mat4 skyView = glm::mat4(glm::mat3(camera.GetViewMatrix())); // Lock to player
        
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(sunModel));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(skyView));
        glUniform3f(sunColorLoc, 1.0f, 1.0f, 1.0f);

        glBindVertexArray(sunVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glEnable(GL_DEPTH_TEST);  // Turn depth back on for 3D blocks
        glUniform1i(isSunLoc, 0); // Tell shader we are back to drawing textures

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniform3fv(viewPosLoc, 1, glm::value_ptr(camera.Position));
        glUniform3fv(skyColorLoc, 1, glm::value_ptr(skyColor));

        for (auto const& [coord, chunk] : activeChunks) {
            glBindVertexArray(chunk->VAO);
            glm::mat4 chunkModel = glm::mat4(1.0f); 
            chunkModel = glm::translate(chunkModel, glm::vec3(chunk->chunkX * CHUNK_WIDTH, 0.0f, chunk->chunkZ * CHUNK_WIDTH));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(chunkModel));

            int vertexCount = chunk->meshVertices.size() / 3;
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        }

        glDisable(GL_DEPTH_TEST); // Draw on top of blocks
        glUseProgram(ui.uiShaderProgram); 
        
        glBindVertexArray(ui.crosshairVAO);
        glDrawArrays(GL_LINES, 0, 4); 

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
uint8_t getBlockAt(int globalX, int globalY, int globalZ) {
    // If we are above or below the world boundaries, treat it as air
    if (globalY < 0 || globalY >= CHUNK_HEIGHT) return 0; 

    int chunkX = static_cast<int>(std::floor(static_cast<float>(globalX) / CHUNK_WIDTH));
    int chunkZ = static_cast<int>(std::floor(static_cast<float>(globalZ) / CHUNK_WIDTH));
    
    std::pair<int, int> coord(chunkX, chunkZ);
    
    // If the chunk isn't loaded yet, treat it as air so we don't crash
    if (activeChunks.find(coord) == activeChunks.end()) return 0; 
    
    // Convert global coordinates to local 0-63 chunk coordinates
    int localX = globalX - (chunkX * CHUNK_WIDTH);
    int localZ = globalZ - (chunkZ * CHUNK_WIDTH);
    
    return activeChunks[coord]->chunkData[localX][globalY][localZ];
}
bool checkCollision(glm::vec3 pos) {
    // Define the player's hitbox dimensions
    float playerWidth = 0.6f; 
    float playerHeight = 1.8f;
    float eyeHeight = 1.5f; // Distance from feet to the camera/eyes

    float minX = pos.x - (playerWidth / 2.0f);
    float maxX = pos.x + (playerWidth / 2.0f);
    float minY = pos.y - eyeHeight;
    float maxY = pos.y + (playerHeight - eyeHeight);
    float minZ = pos.z - (playerWidth / 2.0f);
    float maxZ = pos.z + (playerWidth / 2.0f);

    // Round down to check all integer block coordinates the box overlaps
    int bMinX = static_cast<int>(std::floor(minX));
    int bMaxX = static_cast<int>(std::floor(maxX));
    int bMinY = static_cast<int>(std::floor(minY));
    int bMaxY = static_cast<int>(std::floor(maxY));
    int bMinZ = static_cast<int>(std::floor(minZ));
    int bMaxZ = static_cast<int>(std::floor(maxZ));

    // Loop through the overlapping blocks
    for (int x = bMinX; x <= bMaxX; x++) {
        for (int y = bMinY; y <= bMaxY; y++) {
            for (int z = bMinZ; z <= bMaxZ; z++) {
                uint8_t blockID = getBlockAt(x, y, z);
                
                // If it's not Air(0), Water(2), or Lava(11), it's a solid collision!
                if (blockID != 0 && blockID != 2 && blockID != 11) {
                    return true; 
                }
            }
        }
    }
    return false; // Path is clear
}
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
        
    glm::vec3 oldPos = camera.Position;

    float currentSpeed = camera.MovementSpeed * deltaTime;
    static double lastBreakTime = 0.0;

    static double lastPlaceTime = 0.0;

    double currentTime = glfwGetTime();
    glm::vec3 flatFront = glm::normalize(glm::vec3(camera.Front.x, 0.0f, camera.Front.z));
    glm::vec3 flatRight = glm::normalize(glm::vec3(camera.Right.x, 0.0f, camera.Right.z));

    glm::vec3 horizontalDelta(0.0f);
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) horizontalDelta += flatFront * currentSpeed;
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) horizontalDelta -= flatFront * currentSpeed;
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) horizontalDelta -= flatRight * currentSpeed;
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) horizontalDelta += flatRight * currentSpeed;
    if(glfwGetKey(window,GLFW_KEY_1) == GLFW_PRESS) actions.block_type = 1;
    if(glfwGetKey(window,GLFW_KEY_2) == GLFW_PRESS) actions.block_type = 3;
    if(glfwGetKey(window,GLFW_KEY_3) == GLFW_PRESS) actions.block_type = 4;
    if(glfwGetKey(window,GLFW_KEY_4) == GLFW_PRESS) actions.block_type = 5;
    if(glfwGetKey(window,GLFW_KEY_5) == GLFW_PRESS) actions.block_type = 6;
    if(glfwGetKey(window,GLFW_KEY_6) == GLFW_PRESS) actions.block_type = 7;
    if(glfwGetKey(window,GLFW_KEY_7) == GLFW_PRESS) actions.block_type = 8;
    if(glfwGetKey(window,GLFW_KEY_8) == GLFW_PRESS) actions.block_type = 9;
    if(glfwGetKey(window,GLFW_KEY_9) == GLFW_PRESS) actions.block_type = 10;
    camera.Position.x += horizontalDelta.x;
    if (checkCollision(camera.Position)) camera.Position.x = oldPos.x;

    camera.Position.z += horizontalDelta.z;
    if (checkCollision(camera.Position)) camera.Position.z = oldPos.z;

    float gravity = -25.0f; // Blocks per second squared
    float jumpForce = 9.0f; // Initial upward burst

    camera.verticalVelocity += gravity * deltaTime;

    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && camera.isGrounded) {
        camera.verticalVelocity = jumpForce;
        camera.isGrounded = false; 
    }

    camera.Position.y += camera.verticalVelocity * deltaTime;

    camera.isGrounded = false; 

    if (checkCollision(camera.Position)) {
        camera.Position.y = oldPos.y; // Revert position to avoid clipping
        
        if (camera.verticalVelocity < 0.0f) {
            camera.isGrounded = true; 
        }
        
        camera.verticalVelocity = 0.0f; 
    }

    camera.xCoords = camera.Position.x;
    camera.yCoords = camera.Position.y;
    camera.zCoords = camera.Position.z;

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
        if (currentTime - lastBreakTime > 0.2) { 
            RaycastResult ray = actions.getLookingAt(camera.Position, glm::normalize(camera.Front), 5.0f, activeChunks);
            if(ray.hit){
                actions.breakBlock(ray, activeChunks);
            }
            lastBreakTime = currentTime; 
        }
    }

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS){
        if (currentTime - lastPlaceTime > 0.2) {
            RaycastResult ray = actions.getLookingAt(camera.Position, glm::normalize(camera.Front), 5.0f, activeChunks);
            if(ray.hit){
                actions.placeBlock(ray, activeChunks);
            }
            lastPlaceTime = currentTime; 
        }
    }

    static bool f11PressedLastFrame = false;
    static bool isFullscreen = false;
    static int winPosX, winPosY, winWidth, winHeight; 

    bool f11Pressed = glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS;
    
    if (f11Pressed && !f11PressedLastFrame) {
        isFullscreen = !isFullscreen;

        if (isFullscreen) {
            glfwGetWindowPos(window, &winPosX, &winPosY);
            glfwGetWindowSize(window, &winWidth, &winHeight);
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        } else {
            glfwSetWindowMonitor(window, nullptr, winPosX, winPosY, winWidth, winHeight, 0);
        }
    }
    
    f11PressedLastFrame = f11Pressed;
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
    glViewport(0, 0, width, height);
}
