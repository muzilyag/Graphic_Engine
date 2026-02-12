#ifndef MESH_H
#define MESH_H
#include "GLLoader.h"
#include "Logger.h"

class Mesh
{
    Logger logger;
    unsigned int VAO, VBO;
    int vertexCount;
public:
    Mesh(float* vertices, int size);
    ~Mesh();
    void draw();
};
#endif