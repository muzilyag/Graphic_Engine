#ifndef MESH_H
#define MESH_H
#include <vector>
#include "engine/platform/GLLoader.h"
#include "engine/math/Math3d.h"
#include "engine/utils/Logger.h"

struct Vertex
{
    Vec3 position;
    Vec3 normal;
    float u;
    float v;
};

class Mesh
{
    Logger logger;
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
    int indexCount = 0;
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void draw() const;
};
#endif