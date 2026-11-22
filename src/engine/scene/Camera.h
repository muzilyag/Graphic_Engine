#ifndef CAMERA_H
#define CAMERA_H
#include "engine/math/Math3d.h"

class Camera
{
    Vec3 pos;
    Vec3 front;
    Vec3 up;
    float yaw, pitch;
    bool firstMouse;
    float lastX, lastY;
    
    void updateVectors();
public:
    Camera();
    ~Camera() = default;
    void processKeyboard(bool *keys, float dt);
    void processMouse(float xPos, float yPos);
    // Getters
    Mat4 getViewMatrix();
    float getLastX() { return lastX; }
    float getLastY() { return lastY; }
    Vec3 getPos() { return pos; }
    // Setters
    void setLastX(const float& inLastX) { lastX = inLastX; }
    void setLastY(const float& inLastY) { lastY = inLastY; }
};
#endif