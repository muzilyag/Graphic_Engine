#ifndef MATERIAL_H
#define MATERIAL_H
#include <memory>
#include "engine/math/Math3d.h"
#include "engine/renderer/Texture.h"

struct Material
{
    Vec3 ambient{0.1f, 0.1f, 0.1f};
    Vec3 diffuse{1.0f, 1.0f, 1.0f};
    Vec3 specular{0.5f, 0.5f, 0.5f};
    float shininess = 32.0f;

    std::shared_ptr<Texture> diffuseMap = nullptr;
    std::shared_ptr<Texture> specularMap = nullptr;
    std::shared_ptr<Texture> normalMap = nullptr;
};

struct PointLight
{
    Vec3 position{0.0f, 0.0f, 0.0f};
    Vec3 color{1.0f, 1.0f, 1.0f};
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};
#endif