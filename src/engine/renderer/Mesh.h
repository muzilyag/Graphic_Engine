#ifndef MESH_H
#define MESH_H
#include <cstddef>
#include <vector>
#include "engine/platform/GLLoader.h"
#include "engine/math/Bounds.h"
#include "engine/math/Math3d.h"
#include "engine/utils/Logger.h"

struct Vertex
{
    Vec3 position;
    Vec3 normal;
    float u;
    float v;
    Vec3 tangent{0.0f, 0.0f, 0.0f};
    Vec3 bitangent{0.0f, 0.0f, 0.0f};
};

class Mesh
{
    Logger logger;
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
    unsigned int instanceVBO = 0;
    size_t instanceCapacity = 0;
    int indexCount = 0;
    AABB bounds;
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    const AABB& getBounds() const { return bounds; }
    void draw() const;
    void drawInstanced(const std::vector<Mat4>& matrices);
};
#endif