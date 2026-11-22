#ifndef MATH3D_H
#define MATH3D_H
#include <cmath>

struct Vec3 
{
    float x, y, z;
    Vec3 operator+(const Vec3& b) const;
    Vec3 operator-(const Vec3& b) const;
    Vec3 operator*(float b) const;
    static float dot(const Vec3& a, const Vec3& b);
    static Vec3 cross(const Vec3& a, const Vec3& b);
    static Vec3 normalize(const Vec3& v);
    static Vec3 lerp(const Vec3& a, const Vec3& b, float t);
};

struct Quat
{
    float x, y, z, w;
    Quat();
    Quat(float inX, float inY, float inZ, float inW);
    static Quat identity();
    static Quat fromAxisAngle(const Vec3& axis, float angle);
    static Quat multiply(const Quat& a, const Quat& b);
    static Quat normalize(const Quat& q);
    static Quat slerp(const Quat& a, const Quat& b, float t);
    float dot(const Quat& q) const;
};

struct Mat4 
{
    float m[16];
    Mat4();
    static Mat4 identity();
    static Mat4 multiply(const Mat4& a, const Mat4& b);
    static Mat4 rotateX(float a);
    static Mat4 rotateY(float a);
    static Mat4 rotateZ(float a);
    static Mat4 translate(float x, float y, float z);
    static Mat4 perspective(float fov, float asp, float n, float f);
    static Mat4 ortho(float l, float r, float b, float t, float n, float f);
    static Mat4 lookAt(Vec3 eye, Vec3 target, Vec3 up);
    static Mat4 scale(float x, float y, float z);
    static Mat4 fromQuat(const Quat& q);
};
#endif