#ifndef SCENE_H
#define SCENE_H
#include <memory>
#include <string>
#include <vector>
#include "engine/math/Math3d.h"
#include "engine/renderer/Material.h"
#include "engine/renderer/Renderer.h"
#include "engine/scene/Entity.h"

struct InstancedBatch
{
    std::string name;
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Shader> shader;
    Material material;
    std::vector<Mat4> transforms;
    bool castsShadows = true;
};

class Scene
{
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<InstancedBatch>> instancedBatches;
    std::vector<PointLight> pointLights;
    DirectionalLight directionalLight;
public:
    Scene() = default;
    ~Scene() = default;

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    Entity& createEntity(const std::string& name, std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader, const Material& material);
    Entity* findEntity(const std::string& name);

    InstancedBatch& createInstancedBatch(const std::string& name, std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader, const Material& material, bool castsShadows = true);
    InstancedBatch* findInstancedBatch(const std::string& name);

    void addPointLight(const PointLight& light);
    std::vector<PointLight>& getPointLights() { return pointLights; }
    const std::vector<PointLight>& getPointLights() const { return pointLights; }

    void setDirectionalLight(const DirectionalLight& light);
    DirectionalLight& getDirectionalLight() { return directionalLight; }
    const DirectionalLight& getDirectionalLight() const { return directionalLight; }

    void submit(Renderer& renderer) const;
    void clear();
};
#endif