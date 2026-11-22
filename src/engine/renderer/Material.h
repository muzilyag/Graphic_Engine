#ifndef MATERIAL_H
#define MATERIAL_H
#include <memory>
#include "engine/math/Math3d.h"
#include "engine/renderer/Texture.h"

struct Material
{
    Vec3 albedo{1.0f, 1.0f, 1.0f};
    float metallic = 0.0f;
    float roughness = 0.5f;
    float ao = 1.0f;
    Vec3 emissive{0.0f, 0.0f, 0.0f};
    float opacity = 1.0f;
    float alphaCutoff = 0.0f;
    bool transparent = false;
    bool doubleSided = false;

    std::shared_ptr<Texture> albedoMap = nullptr;
    std::shared_ptr<Texture> normalMap = nullptr;
    std::shared_ptr<Texture> metallicMap = nullptr;
    std::shared_ptr<Texture> roughnessMap = nullptr;
    std::shared_ptr<Texture> aoMap = nullptr;
};

struct PointLight
{
    Vec3 position{0.0f, 0.0f, 0.0f};
    Vec3 color{1.0f, 1.0f, 1.0f};
    float intensity = 15.0f;
    float radius = 20.0f;
};

struct DirectionalLight
{
    Vec3 direction{-0.45f, -0.65f, 0.6f};
    Vec3 color{1.0f, 0.95f, 0.88f};
    float intensity = 3.0f;
    bool enabled = false;
};
#endif