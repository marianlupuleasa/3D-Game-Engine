#ifndef CAMERA_H
#define CAMERA_H

#include <mat4.h>
#include <vec3.h>

#define M_PI 3.14

inline float degreeToRadians(float degrees) 
{
    return degrees * (M_PI / 180.0f);
}

enum Camera_Movement 
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera 
{
public:
    Vec3 position;
    Vec3 front;
    Vec3 up;
    Vec3 right;
    Vec3 worldUp;

    float yaw;
    float pitch;
    float mouseSensitivity;

    // Default Camera Position
    Camera(Vec3 position = Vec3(0.0f, 0.0f, 0.0f),
        Vec3 up = Vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f, 
        float pitch = 0.0f)
        : position(position), 
        worldUp(up), 
        yaw(yaw), 
        pitch(pitch),
        front(Vec3(0.0f, 0.0f, -1.0f)),
        mouseSensitivity(0.1f)
    {
        UpdateCameraVectors();
    }

    ~Camera()
    {

    }

    // Look toward -Oz axis
    Mat4 GetViewMatrix() const
    {

        Vec3 center = position + front;

        return Mat4().LookAt(position, center, up);
    }

    // Modify position based on direction given from keyboard input
    void ProcessKeyboard(Camera_Movement direction, float deltaTime) 
    {
        float velocity = deltaTime;
        if (direction == FORWARD)
            position = position + front * velocity;
        if (direction == BACKWARD)
            position = position - front * velocity;
        if (direction == LEFT)
            position = position - right * velocity;
        if (direction == RIGHT)
            position = position + right * velocity;
        if (direction == UP)
            position = position + worldUp * velocity;
        if (direction == DOWN)
            position = position - worldUp * velocity;
    }

    // Rotate camera based on mouse movement
    void ProcessMouseMovement(float offsetX, float offsetY) 
    {
        offsetX *= mouseSensitivity;
        offsetY *= mouseSensitivity;

        yaw += offsetX;
        pitch += offsetY;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        UpdateCameraVectors();
    }
    
    // Recalculate vectors to update current camera angle
    void UpdateCameraVectors() 
    {
        Vec3 front;
        front.x = cos(degreeToRadians(yaw)) * cos(degreeToRadians(pitch));
        front.y = sin(degreeToRadians(pitch));
        front.z = sin(degreeToRadians(yaw)) * cos(degreeToRadians(pitch));

        this->front = front.Normalize();
        right = (this->front.Cross(worldUp)).Normalize();
        up = (right.Cross(this->front)).Normalize();
    }
};

#endif