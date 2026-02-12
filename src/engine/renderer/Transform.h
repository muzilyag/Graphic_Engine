#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "Math3d.h"

class Transform
{
    Vec3 position;
    // Vec3 rotation;
    Quat rotation;
    Vec3 scale;
public:
    Transform();
    //Getters
    Mat4 getModelMatrix();
    // Setters
    void setPosiotn(const Vec3& inPos) { position = inPos; }
    void setRotation(const Quat& inRot) { rotation = inRot;}
    void setRotation(const Vec3& inRot);
    void setScale(const Vec3& inScale) { scale = inScale;}

    void lerpTo(const Vec3& targetPos, const Quat& targetRot, float t);
    void rotate(const Vec3& axis, float angle);
};

#endif