#ifndef SANDBOXAPP_H
#define SANDBOXAPP_H
#include <vector>
#include "engine/core/Application.h"

struct OrbitInstanceData
{
    float radius = 20.0f;
    float baseAngle = 0.0f;
    float orbitSpeed = 0.1f;
    float height = 0.0f;
    float bobAmp = 0.5f;
    float bobSpeed = 1.0f;
    float rotSpeedY = 0.5f;
    float rotSpeedX = 0.3f;
    float scale = 0.5f;
};

class SandboxApp : public Application
{
    Entity* sphereEntity = nullptr;
    Entity* pyramidEntity = nullptr;
    std::vector<Entity*> cubeEntities;
    std::vector<Entity*> lightEntities;

    InstancedBatch* instancedCubes = nullptr;
    InstancedBatch* instancedSpheres = nullptr;
    std::vector<OrbitInstanceData> cubeOrbitData;
    std::vector<OrbitInstanceData> sphereOrbitData;

    bool flashlightEnabled = true;

    void updateInstancedOrbit(InstancedBatch* batch, const std::vector<OrbitInstanceData>& data);
protected:
    void onInit() override;
    void onUpdate(float dt) override;
public:
    SandboxApp();
};
#endif