#ifndef PRIMITIVES_H
#define PRIMITIVES_H
#include <memory>
#include "engine/renderer/Mesh.h"

class Primitives
{
public:
    static std::unique_ptr<Mesh> createCube();
    static std::unique_ptr<Mesh> createPlane(float size = 10.0f, float uvScale = 1.0f);
    static std::unique_ptr<Mesh> createSphere(float radius = 0.5f, unsigned int segments = 32, unsigned int rings = 16);
};
#endif