#include "engine/scene/Camera.h"

#define SPACE_KEY 0x20
#define SHIFT_KEY 0x10

Camera::Camera()
{
    pos = {0.f, 0.f, 3.f};
    front = {0.f, 0.f, -1.f};
    up = {0.f, 1.f, 0.f};
    yaw = -90.f;
    pitch = 0.f;
    lastX = 400.f;
    lastY = 300.f;
}

Mat4 Camera::getViewMatrix() const
{
    return Mat4::lookAt(pos, pos + front, up);
}

void Camera::processKeyboard(bool *keys, float dt)
{
    float speed = 2.5f * dt;
    Vec3 right = Vec3::normalize(Vec3::cross(front, up));
    Vec3 moveDir = {0.0f, 0.0f, 0.0f};

    if (keys['W'])
    {
        moveDir = moveDir + front;
    }
    if (keys['A'])
    {
        moveDir = moveDir - right;
    }
    if (keys['S'])
    {
        moveDir = moveDir - front;
    }
    if (keys['D'])
    {
        moveDir = moveDir + right;
    }
    if (keys[SPACE_KEY])
    {
        moveDir.y += 1.0f;
    }
    if (keys[SHIFT_KEY])
    {
        moveDir.y -= 1.0f;
    }

    pos = pos + Vec3::normalize(moveDir) * speed;
}

void Camera::processMouse(float xPos, float yPos)
{
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
    constexpr float DEG2RAD = 0.0174532925f;
    Vec3 f;
    f.x = cosf(yaw * DEG2RAD) * cosf(pitch * DEG2RAD);
    f.y = sinf(pitch * DEG2RAD);
    f.z = sinf(yaw * DEG2RAD) * cosf(pitch * DEG2RAD);
    front = Vec3::normalize(f);
}