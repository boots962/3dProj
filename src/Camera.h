#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Default camera values

const float YAW         = -90.0f;
const float PITCH       = -20.0f; // Look slightly down by default
const float SPEED       = 10.0f;  // Adjusted for a voxel environment
const float SENSITIVITY = 0.1f;


class Camera
{
public:
    float verticalVelocity = 0.0f;
    bool isGrounded = false;
    // Camera Attributes
    glm::vec3 looking_at;
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    
    float xCoords = 0.0f, yCoords = 0.0f, zCoords = 0.0f;;   
    
    // Euler Angles
    float Yaw;
    float Pitch;
    
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) 
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // Processes input received from a keyboard
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    
    // Create a flattened version of the Front vector
    glm::vec3 flatFront = glm::normalize(glm::vec3(Front.x, 0.0f, Front.z));
    // Create a flattened version of the Right vector
    glm::vec3 flatRight = glm::normalize(glm::vec3(Right.x, 0.0f, Right.z));

    if (direction == FORWARD)
        Position += flatFront * velocity;
    if (direction == BACKWARD)
        Position -= flatFront * velocity;
    if (direction == LEFT)
        Position -= flatRight * velocity;
    if (direction == RIGHT)
        Position += flatRight * velocity;
    
    // UP and DOWN can stay the same, or you can map UP to a "Jump" variable later!
    if(direction == UP)
        Position += WorldUp * velocity;
    if(direction == DOWN)
        Position -= WorldUp * velocity;

    xCoords = Position[0];
    yCoords = Position[1];
    zCoords = Position[2];
}

    // Processes input received from a mouse input system
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        looking_at = {front.x, front.y, front.z};
        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp)); 
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};