#ifndef SCENE_H
#define SCENE_H
#include <memory>
#include <string>
#include <vector>
#include "engine/renderer/Material.h"
#include "engine/renderer/Renderer.h"
#include "engine/scene/Entity.h"

class Scene
{
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<PointLight> pointLights;
public:
    Scene() = default;
    ~Scene() = default;

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    Entity& createEntity(const std::string& name, std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader, const Material& material);
    Entity* findEntity(const std::string& name);

    void addPointLight(const PointLight& light);
    std::vector<PointLight>& getPointLights() { return pointLights; }
    const std::vector<PointLight>& getPointLights() const { return pointLights; }

    void submit(Renderer& renderer) const;
    void clear();
};
#endif