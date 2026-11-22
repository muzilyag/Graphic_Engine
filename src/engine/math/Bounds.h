#ifndef BOUNDS_H
#define BOUNDS_H
#include "engine/math/Math3d.h"

struct AABB
{
    Vec3 min{0.0f, 0.0f, 0.0f};
    Vec3 max{0.0f, 0.0f, 0.0f};

    AABB transformed(const Mat4& mat) const;
};

struct Plane
{
    Vec3 normal{0.0f, 1.0f, 0.0f};
    float d = 0.0f;
};

struct Frustum
{
    Plane planes[6];

    static Frustum fromViewProjection(const Mat4& vp);
    bool intersects(const AABB& box) const;
};
#endif