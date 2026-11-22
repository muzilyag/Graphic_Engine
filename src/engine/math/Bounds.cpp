#include "engine/math/Bounds.h"
#include <cmath>

namespace
{
    Plane makeNormalizedPlane(float a, float b, float c, float d)
    {
        float lenSq = a * a + b * b + c * c;
        if (lenSq > 1e-12f)
        {
            float invLen = 1.0f / sqrtf(lenSq);
            return { { a * invLen, b * invLen, c * invLen }, d * invLen };
        }
        return { { 0.0f, 0.0f, 0.0f }, 0.0f };
    }
}

AABB AABB::transformed(const Mat4& mat) const
{
    Vec3 c = (min + max) * 0.5f;
    Vec3 e = (max - min) * 0.5f;

    Vec3 worldCenter = {
        mat.m[0] * c.x + mat.m[4] * c.y + mat.m[8]  * c.z + mat.m[12],
        mat.m[1] * c.x + mat.m[5] * c.y + mat.m[9]  * c.z + mat.m[13],
        mat.m[2] * c.x + mat.m[6] * c.y + mat.m[10] * c.z + mat.m[14]
    };

    Vec3 worldExtents = {
        fabsf(mat.m[0]) * e.x + fabsf(mat.m[4]) * e.y + fabsf(mat.m[8])  * e.z,
        fabsf(mat.m[1]) * e.x + fabsf(mat.m[5]) * e.y + fabsf(mat.m[9])  * e.z,
        fabsf(mat.m[2]) * e.x + fabsf(mat.m[6]) * e.y + fabsf(mat.m[10]) * e.z
    };

    return { worldCenter - worldExtents, worldCenter + worldExtents };
}

Frustum Frustum::fromViewProjection(const Mat4& vp)
{
    Frustum f;

    f.planes[0] = makeNormalizedPlane(vp.m[3] + vp.m[0], vp.m[7] + vp.m[4], vp.m[11] + vp.m[8],  vp.m[15] + vp.m[12]);
    f.planes[1] = makeNormalizedPlane(vp.m[3] - vp.m[0], vp.m[7] - vp.m[4], vp.m[11] - vp.m[8],  vp.m[15] - vp.m[12]);
    f.planes[2] = makeNormalizedPlane(vp.m[3] + vp.m[1], vp.m[7] + vp.m[5], vp.m[11] + vp.m[9],  vp.m[15] + vp.m[13]);
    f.planes[3] = makeNormalizedPlane(vp.m[3] - vp.m[1], vp.m[7] - vp.m[5], vp.m[11] - vp.m[9],  vp.m[15] - vp.m[13]);
    f.planes[4] = makeNormalizedPlane(vp.m[3] + vp.m[2], vp.m[7] + vp.m[6], vp.m[11] + vp.m[10], vp.m[15] + vp.m[14]);
    f.planes[5] = makeNormalizedPlane(vp.m[3] - vp.m[2], vp.m[7] - vp.m[6], vp.m[11] - vp.m[10], vp.m[15] - vp.m[14]);

    return f;
}

bool Frustum::intersects(const AABB& box) const
{
    Vec3 c = (box.min + box.max) * 0.5f;
    Vec3 e = (box.max - box.min) * 0.5f;

    for (int i = 0; i < 6; ++i)
    {
        const Plane& p = planes[i];
        float r = e.x * fabsf(p.normal.x) + e.y * fabsf(p.normal.y) + e.z * fabsf(p.normal.z);
        float dist = Vec3::dot(p.normal, c) + p.d;
        if (dist + r < 0.0f)
        {
            return false;
        }
    }

    return true;
}