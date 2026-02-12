#include "Transform.h"

Transform::Transform()
{
    position = {0.f, 0.f, 0.f};
    // rotation = {0.f, 0.f, 0.f};
    rotation = Quat::identity();
    scale    = {1.f, 1.f, 1.f};
}

void Transform::setRotation(const Vec3& inRot)
{
    Quat qx = Quat::fromAxisAngle({1,0,0}, inRot.x);
    Quat qy = Quat::fromAxisAngle({0,1,0}, inRot.y);
    Quat qz = Quat::fromAxisAngle({0,0,1}, inRot.z);
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

Mat4 Transform::getModelMatrix()
{
    Mat4 pos = Mat4::translate(position.x, position.y, position.z);
    
    Mat4 rotM = Mat4::fromQuat(rotation);
    // Mat4 rotX = Mat4::rotateX(rotation.x);
    // Mat4 rotY = Mat4::rotateY(rotation.y);
    // Mat4 rotZ = Mat4::rotateZ(rotation.z);
    // Mat4 rot = Mat4::multiply(rotY, Mat4::multiply(rotX, rotZ));

    Mat4 scl = Mat4::scale(scale.x, scale.y, scale.z);

    return Mat4::multiply(pos, Mat4::multiply(rotM, scl)); // вместо rotM был rot
}