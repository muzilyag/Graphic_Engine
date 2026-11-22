#ifndef SANDBOXAPP_H
#define SANDBOXAPP_H
#include <vector>
#include "engine/core/Application.h"

class SandboxApp : public Application
{
    Entity* sphereEntity = nullptr;
    Entity* pyramidEntity = nullptr;
    std::vector<Entity*> cubeEntities;
protected:
    void onInit() override;
    void onUpdate(float dt) override;
public:
    SandboxApp();
};
#endif