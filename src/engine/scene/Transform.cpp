#include "engine/scene/Transform.h"

Transform::Transform()
{
    position = {0.0f, 0.0f, 0.0f};
    rotation = Quat::identity();
    scale    = {1.0f, 1.0f, 1.0f};
}

void Transform::setRotation(const Vec3& inRot)
{
    Quat qx = Quat::fromAxisAngle({1.0f, 0.0f, 0.0f}, inRot.x);
    Quat qy = Quat::fromAxisAngle({0.0f, 1.0f, 0.0f}, inRot.y);
    Quat qz = Quat::fromAxisAngle({0.0f, 0.0f, 1.0f}, inRot.z);
    rotation = Quat::multiply(qy, Quat::multiply(qx, qz));
}

void Transform::lerpTo(const Vec3& targetPos, const Quat& targetRot, float t)
{
    position = Vec3::lerp(position, targetPos, t);
    rotation = Quat::slerp(rotation, targetRot, t);
}

void Transform::rotate(const Vec3& axis, float angle)
{
    Quat delta = Quat::fromAxisAngle(axis, angle);
    rotation = Quat::normalize(Quat::multiply(rotation, delta));
}

Mat4 Transform::getModelMatrix() const
{
    return Mat4::trs(position, rotation, scale);
}