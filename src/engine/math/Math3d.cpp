#include "engine/math/Math3d.h"

Vec3 Vec3::operator+(const Vec3& b) const
{   
    return {x + b.x, y + b.y, z + b.z};
}
Vec3 Vec3::operator-(const Vec3& b) const
{
    return {x - b.x, y - b.y, z - b.z};
}
Vec3 Vec3::operator*(float b) const
{
    return {x * b, y * b, z * b};
}
float Vec3::dot(const Vec3& a, const Vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
Vec3 Vec3::cross(const Vec3& a, const Vec3& b)
{
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.z};
}
Vec3 Vec3::normalize(const Vec3& v)
{
    float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    return {v.x / len, v.y / len, v.z / len};
}
Vec3 Vec3::lerp(const Vec3& a, const Vec3& b, float t)
{
    return a * (1.f - t) + b * t;
}


Mat4::Mat4() 
{ 
    for (int i = 0; i < 16; i++) 
    {
        m[i] = 0.0f; 
    }
}

Mat4 Mat4::identity() 
{
    Mat4 res; 
    res.m[0] = res.m[5] = res.m[10] = res.m[15] = 1.0f;
    return res;
}

Mat4 Mat4::multiply(const Mat4& a, const Mat4& b) 
{
    Mat4 res;
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            res.m[c * 4 + r] =  a.m[0 * 4 + r] * b.m[c * 4 + 0] + 
                                a.m[1 * 4 + r] * b.m[c * 4 + 1] + 
                                a.m[2 * 4 + r] * b.m[c * 4 + 2] + 
                                a.m[3 * 4 + r] * b.m[c * 4 + 3];
        }
    }
    return res;
}

Mat4 Mat4::rotateX(float a) 
{
    Mat4 r = identity(); 
    r.m[5] = cosf(a); r.m[6] = sinf(a); r.m[9] = -sinf(a); r.m[10] = cosf(a);
    return r;
}

Mat4 Mat4::rotateY(float a) 
{
    Mat4 r = identity(); 
    r.m[0] = cosf(a); r.m[2] = -sinf(a); r.m[8] = sinf(a); r.m[10] = cosf(a);
    return r;
}

Mat4 Mat4::rotateZ(float a) 
{
    Mat4 r = identity(); 
    r.m[0] = cosf(a); r.m[1] = sinf(a); r.m[4] = -sinf(a); r.m[5] = cosf(a);
    return r;
}

Mat4 Mat4::translate(float x, float y, float z) 
{
    Mat4 r = identity(); 
    r.m[12] = x; r.m[13] = y; r.m[14] = z;
    return r;
}

Mat4 Mat4::perspective(float fov, float asp, float n, float f) 
{
    Mat4 r; 
    float thf = tanf(fov / 2.0f);
    r.m[0] = 1.0f / (asp * thf); r.m[5] = 1.0f / thf; r.m[10] = -(f + n) / (f - n); r.m[11] = -1.0f; r.m[14] = -(2.0f * f * n) / (f - n);
    return r;
}

Mat4 Mat4::lookAt(Vec3 eye, Vec3 target, Vec3 up)
{
    Vec3 f = Vec3::normalize(target - eye);
    Vec3 s = Vec3::normalize(Vec3::cross(f, up));
    Vec3 u = Vec3::cross(s, f);
    Mat4 res = identity();
    res.m[0] = s.x; res.m[4] = s.y; res.m[8] = s.z;
    res.m[1] = u.x; res.m[5] = u.y; res.m[9] = u.z;
    res.m[2] = -f.x; res.m[6] = -f.y; res.m[10] = -f.z;
    res.m[12] = -Vec3::dot(s, eye); 
    res.m[13] = -Vec3::dot(u, eye);
    res.m[14] = Vec3::dot(f, eye);
    return res;
}

Mat4 Mat4::scale(float x, float y, float z)
{
    Mat4 res = identity();
    res.m[0] = x; res.m[5] = y; res.m[10] = z;
    return res;

}

Mat4 Mat4::fromQuat(const Quat& q)
{
    Mat4 r = identity();
    float xx = q.x * q.x; float yy = q.y * q.y; float zz = q.z * q.z;
    float xy = q.x * q.y; float xz = q.x * q.z; float yz = q.y * q.z;
    float wx = q.w * q.x; float wy = q.w * q.y; float wz = q.w * q.z;

    r.m[0] = 1.0f - 2.0f * (yy + zz);
    r.m[1] = 2.0f * (xy + wz);
    r.m[2] = 2.0f * (xz - wy);

    r.m[4] = 2.0f * (xy - wz);
    r.m[5] = 1.0f - 2.0f * (xx + zz);
    r.m[6] = 2.0f * (yz + wx);

    r.m[8] = 2.0f * (xz + wy);
    r.m[9] = 2.0f * (yz - wx);
    r.m[10] = 1.0f - 2.0f * (xx + yy);
    return r;
}

Quat::Quat()
{
    x = y = z = 0;
    w = 1;
}

Quat::Quat(float inX, float inY, float inZ, float inW)
{
    x = inX; y = inY; z = inZ; w = inW;
}

Quat Quat::identity()
{
    return Quat(0, 0, 0, 1);
}

Quat Quat::fromAxisAngle(const Vec3& axis, float angle)
{
    float halfAngle = angle * 0.5f;
    float s = sinf(halfAngle);
    Vec3 n = Vec3::normalize(axis);
    return Quat(n.x * s, n.y * s, n.z * s, cosf(halfAngle));
}

Quat Quat::multiply(const Quat& a, const Quat& b)
{
    return Quat(
        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
    );
}

float Quat::dot(const Quat& q) const
{
    return x * q.x + y * q.y + z * q.z + w * q.w;
}

Quat Quat::normalize(const Quat& q)
{
    float len = sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    return Quat(q.x / len, q.y / len, q.z / len, q.w / len);
}

Quat Quat::slerp(const Quat& a, const Quat& b, float t)
{
    float cosTheta = a.dot(b);
    Quat target = b;

    if(cosTheta < 0.0f)
    {
        cosTheta = -cosTheta;
        target = Quat(-b.x, -b.y, -b.z, -b.w);
    }

    if(cosTheta > 0.9995f)
    {
        return normalize(Quat(
            a.x + (target.x - a.x) * t,
            a.y + (target.y - a.y) * t,
            a.z + (target.z - a.z) * t,
            a.w + (target.w - a.w) * t
        ));
    }

    float theta = acosf(cosTheta);
    float sinTheta = sinf(theta);
    float af = sinf((1.0f - t) * theta) / sinTheta;
    float bf = sinf(t * theta) / sinTheta;

    return Quat(
        a.x * af + target.x * bf,
        a.y * af + target.y * bf,
        a.z * af + target.z * bf,
        a.w * af + target.w * bf
    );
}