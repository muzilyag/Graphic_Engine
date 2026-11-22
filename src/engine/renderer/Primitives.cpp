#include "engine/renderer/Primitives.h"
#include <cmath>
#include <vector>

std::unique_ptr<Mesh> Primitives::createCube()
{
    std::vector<Vertex> vertices = {
        {{ 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, 0.0f, 0.0f},
        {{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, 1.0f, 0.0f},
        {{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, 1.0f, 1.0f},
        {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, 0.0f, 1.0f},

        {{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, 0.0f, 0.0f},
        {{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, 1.0f, 0.0f},
        {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, 1.0f, 1.0f},
        {{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, 0.0f, 1.0f},

        {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, 0.0f, 0.0f},
        {{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, 1.0f, 0.0f},
        {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, 1.0f, 1.0f},
        {{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, 0.0f, 1.0f},

        {{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, 0.0f, 0.0f},
        {{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, 1.0f, 0.0f},
        {{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, 1.0f, 1.0f},
        {{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, 0.0f, 1.0f},

        {{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, 0.0f, 0.0f},
        {{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, 1.0f, 0.0f},
        {{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, 1.0f, 1.0f},
        {{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, 0.0f, 1.0f},

        {{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, 0.0f, 0.0f},
        {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, 1.0f, 0.0f},
        {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, 1.0f, 1.0f},
        {{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, 0.0f, 1.0f}
    };

    std::vector<unsigned int> indices;
    indices.reserve(36);
    for (unsigned int face = 0; face < 6; ++face)
    {
        unsigned int base = face * 4;
        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
        indices.push_back(base + 0);
    }

    return std::make_unique<Mesh>(vertices, indices);
}

std::unique_ptr<Mesh> Primitives::createPlane(float size, float uvScale)
{
    float h = size * 0.5f;

    std::vector<Vertex> vertices = {
        {{-h, 0.0f,  h}, {0.0f, 1.0f, 0.0f}, 0.0f,    0.0f},
        {{ h, 0.0f,  h}, {0.0f, 1.0f, 0.0f}, uvScale, 0.0f},
        {{ h, 0.0f, -h}, {0.0f, 1.0f, 0.0f}, uvScale, uvScale},
        {{-h, 0.0f, -h}, {0.0f, 1.0f, 0.0f}, 0.0f,    uvScale}
    };

    std::vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0 };

    return std::make_unique<Mesh>(vertices, indices);
}

std::unique_ptr<Mesh> Primitives::createSphere(float radius, unsigned int segments, unsigned int rings)
{
    constexpr float PI = 3.14159265359f;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    vertices.reserve((rings + 1) * (segments + 1));
    indices.reserve(rings * segments * 6);

    for (unsigned int y = 0; y <= rings; ++y)
    {
        float v = static_cast<float>(y) / static_cast<float>(rings);
        float phi = v * PI;

        for (unsigned int x = 0; x <= segments; ++x)
        {
            float u = static_cast<float>(x) / static_cast<float>(segments);
            float theta = u * 2.0f * PI;

            float nx = cosf(theta) * sinf(phi);
            float ny = cosf(phi);
            float nz = sinf(theta) * sinf(phi);

            vertices.push_back({
                {nx * radius, ny * radius, nz * radius},
                {nx, ny, nz},
                u,
                1.0f - v
            });
        }
    }

    for (unsigned int y = 0; y < rings; ++y)
    {
        for (unsigned int x = 0; x < segments; ++x)
        {
            unsigned int first = y * (segments + 1) + x;
            unsigned int second = first + segments + 1;

            indices.push_back(first);
            indices.push_back(first + 1);
            indices.push_back(second);

            indices.push_back(second);
            indices.push_back(first + 1);
            indices.push_back(second + 1);
        }
    }

    return std::make_unique<Mesh>(vertices, indices);
}