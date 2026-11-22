#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "engine/math/Math3d.h"

class Transform
{
    Vec3 position;
    Quat rotation;
    Vec3 scale;
public:
    Transform();

    Mat4 getModelMatrix() const;
    const Vec3& getPosition() const { return position; }
    const Quat& getRotation() const { return rotation; }
    const Vec3& getScale() const { return scale; }

    void setPosition(const Vec3& inPos) { position = inPos; }
    void setRotation(const Quat& inRot) { rotation = inRot; }
    void setRotation(const Vec3& inRot);
    void setScale(const Vec3& inScale) { scale = inScale; }

    void lerpTo(const Vec3& targetPos, const Quat& targetRot, float t);
    void rotate(const Vec3& axis, float angle);
};
#endif