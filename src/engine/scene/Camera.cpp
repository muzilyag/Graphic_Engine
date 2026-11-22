#include "engine/scene/Camera.h"

#define SPACE_KEY 0x20
#define SHIFT_KEY 0x10

Camera::Camera()
{
    pos = {0.f, 0.f, 3.f};
    front = {0.f, 0.f, -1.f};
    up = {0, 1, 0};
    yaw = -90.f;
    pitch = 0.f;
    lastX = 400;
    lastY = 300;
    firstMouse = true;
}

Mat4 Camera::getViewMatrix()
{
    return Mat4::lookAt(pos, pos + front, up);
}

void Camera::processKeyboard(bool *keys, float dt)
{
    float speed = 2.5f * dt;
    Vec3 right = Vec3::normalize(Vec3::cross(front, up));
    if(keys['W'])
    {
        pos = pos + front * speed;
    }
    if(keys['A'])
    {
        pos = pos - right * speed;
    }
    if(keys['S'])
    {
        pos = pos - front * speed;
    }
    if(keys['D'])
    {
        pos = pos + right * speed;
    }
    if(keys[SPACE_KEY])
    {
        pos.y += speed;
    }
    if(keys[SHIFT_KEY])
    {
        pos.y -= speed;
    }
}

void Camera::processMouse(float xPos, float yPos)
{
    if (firstMouse) 
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos; 
    lastX = xPos;
    lastY = yPos;

    float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw   += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f) 
    {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) 
    {
        pitch = -89.0f;
    }

    updateVectors();
}

void Camera::updateVectors()
{
    Vec3 f;
    f.x = cosf(yaw * 0.0174f) * cosf(pitch * 0.0174f);
    f.y = sinf(pitch * 0.0174f);
    f.z = sinf(yaw * 0.0174f) * cosf(pitch * 0.0174f);
    front = Vec3::normalize(f);
}