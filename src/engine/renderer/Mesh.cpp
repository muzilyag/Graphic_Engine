#include "engine/renderer/Mesh.h"
#include <algorithm>
#include <cmath>
#include <cstddef>

Mesh::Mesh(const std::vector<Vertex>& inputVertices, const std::vector<unsigned int>& indices) : logger("Mesh")
{
    std::vector<Vertex> vertices = inputVertices;
    indexCount = static_cast<int>(indices.size());

    if (!vertices.empty())
    {
        bounds.min = vertices[0].position;
        bounds.max = vertices[0].position;
        for (size_t i = 1; i < vertices.size(); ++i)
        {
            const Vec3& p = vertices[i].position;
            bounds.min.x = std::min(bounds.min.x, p.x);
            bounds.min.y = std::min(bounds.min.y, p.y);
            bounds.min.z = std::min(bounds.min.z, p.z);
            bounds.max.x = std::max(bounds.max.x, p.x);
            bounds.max.y = std::max(bounds.max.y, p.y);
            bounds.max.z = std::max(bounds.max.z, p.z);
        }
    }

    for (size_t i = 0; i + 2 < indices.size(); i += 3)
    {
        unsigned int i0 = indices[i + 0];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];
        if (i0 >= vertices.size() || i1 >= vertices.size() || i2 >= vertices.size())
        {
            continue;
        }

        const Vertex& v0 = vertices[i0];
        const Vertex& v1 = vertices[i1];
        const Vertex& v2 = vertices[i2];

        Vec3 edge1 = v1.position - v0.position;
        Vec3 edge2 = v2.position - v0.position;
        float du1 = v1.u - v0.u;
        float dv1 = v1.v - v0.v;
        float du2 = v2.u - v0.u;
        float dv2 = v2.v - v0.v;

        float det = du1 * dv2 - du2 * dv1;
        if (fabsf(det) < 1e-8f)
        {
            continue;
        }

        float f = 1.0f / det;
        Vec3 t = {
            f * (dv2 * edge1.x - dv1 * edge2.x),
            f * (dv2 * edge1.y - dv1 * edge2.y),
            f * (dv2 * edge1.z - dv1 * edge2.z)
        };
        Vec3 b = {
            f * (-du2 * edge1.x + du1 * edge2.x),
            f * (-du2 * edge1.y + du1 * edge2.y),
            f * (-du2 * edge1.z + du1 * edge2.z)
        };

        vertices[i0].tangent = vertices[i0].tangent + t;
        vertices[i1].tangent = vertices[i1].tangent + t;
        vertices[i2].tangent = vertices[i2].tangent + t;

        vertices[i0].bitangent = vertices[i0].bitangent + b;
        vertices[i1].bitangent = vertices[i1].bitangent + b;
        vertices[i2].bitangent = vertices[i2].bitangent + b;
    }

    for (Vertex& vert : vertices)
    {
        Vec3 n = Vec3::normalize(vert.normal);
        if (Vec3::dot(n, n) < 1e-6f)
        {
            n = { 0.0f, 1.0f, 0.0f };
        }

        Vec3 t = vert.tangent - n * Vec3::dot(n, vert.tangent);
        if (Vec3::dot(t, t) < 1e-8f)
        {
            Vec3 fallback = (fabsf(n.y) < 0.99f) ? Vec3{ 0.0f, 1.0f, 0.0f } : Vec3{ 1.0f, 0.0f, 0.0f };
            t = Vec3::normalize(Vec3::cross(fallback, n));
            vert.tangent = t;
            vert.bitangent = Vec3::normalize(Vec3::cross(n, t));
        }
        else
        {
            t = Vec3::normalize(t);
            Vec3 b = Vec3::cross(n, t);
            float handedness = (Vec3::dot(b, vert.bitangent) < 0.0f) ? -1.0f : 1.0f;
            vert.tangent = t;
            vert.bitangent = Vec3::normalize(b * handedness);
        }
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<ptrdiff_t>(vertices.size() * sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<ptrdiff_t>(indices.size() * sizeof(unsigned int)), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);

    logger.info("Successfully uploaded to GPU. Vertices: " + std::to_string(vertices.size()) + ", Indices: " + std::to_string(indices.size()));
}

Mesh::~Mesh()
{
    if (instanceVBO != 0)
    {
        glDeleteBuffers(1, &instanceVBO);
        instanceVBO = 0;
    }
    if (EBO != 0)
    {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
    if (VBO != 0)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
}

void Mesh::draw() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void Mesh::drawInstanced(const std::vector<Mat4>& matrices)
{
    if (matrices.empty() || !glDrawElementsInstanced || !glVertexAttribDivisor)
    {
        return;
    }

    glBindVertexArray(VAO);

    bool firstInit = (instanceVBO == 0);
    if (firstInit)
    {
        glGenBuffers(1, &instanceVBO);
    }

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

    size_t requiredCount = matrices.size();
    ptrdiff_t byteSize = static_cast<ptrdiff_t>(requiredCount * sizeof(Mat4));
    if (requiredCount > instanceCapacity)
    {
        glBufferData(GL_ARRAY_BUFFER, byteSize, matrices.data(), GL_DYNAMIC_DRAW);
        instanceCapacity = requiredCount;
    }
    else if (glBufferSubData)
    {
        glBufferSubData(GL_ARRAY_BUFFER, 0, byteSize, matrices.data());
    }
    else
    {
        glBufferData(GL_ARRAY_BUFFER, byteSize, matrices.data(), GL_DYNAMIC_DRAW);
    }

    if (firstInit)
    {
        for (unsigned int i = 0; i < 4; ++i)
        {
            glVertexAttribPointer(5 + i, 4, GL_FLOAT, GL_FALSE, sizeof(Mat4), (void*)(sizeof(float) * 4 * i));
            glVertexAttribDivisor(5 + i, 1);
        }
    }

    for (unsigned int i = 0; i < 4; ++i)
    {
        glEnableVertexAttribArray(5 + i);
    }

    glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0, static_cast<int>(requiredCount));

    if (glDisableVertexAttribArray)
    {
        for (unsigned int i = 0; i < 4; ++i)
        {
            glDisableVertexAttribArray(5 + i);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}