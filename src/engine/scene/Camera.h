#ifndef CAMERA_H
#define CAMERA_H
#include "engine/math/Math3d.h"

class Camera
{
    Vec3 pos;
    Vec3 front;
    Vec3 up;
    float yaw, pitch;
    float lastX, lastY;
    
    void updateVectors();
public:
    Camera();
    ~Camera() = default;
    void processKeyboard(bool *keys, float dt);
    void processMouse(float xPos, float yPos);
    // Getters
    Mat4 getViewMatrix() const;
    float getLastX() const { return lastX; }
    float getLastY() const { return lastY; }
    Vec3 getPos() const { return pos; }
    Vec3 getFront() const { return front; }
    // Setters
    void setLastX(const float& inLastX) { lastX = inLastX; }
    void setLastY(const float& inLastY) { lastY = inLastY; }
};
#endif