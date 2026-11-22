#include "engine/scene/Scene.h"
#include <utility>

Entity& Scene::createEntity(const std::string& name, std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader, const Material& material)
{
    auto entity = std::make_unique<Entity>(name, std::move(mesh), std::move(shader), material);
    Entity& ref = *entity;
    entities.push_back(std::move(entity));
    return ref;
}

Entity* Scene::findEntity(const std::string& name)
{
    for (auto& entity : entities)
    {
        if (entity->getName() == name)
        {
            return entity.get();
        }
    }
    return nullptr;
}

void Scene::addPointLight(const PointLight& light)
{
    pointLights.push_back(light);
}

void Scene::submit(Renderer& renderer) const
{
    for (const PointLight& light : pointLights)
    {
        renderer.submitLight(light);
    }

    for (const auto& entity : entities)
    {
        auto mesh = entity->getMesh();
        auto shader = entity->getShader();
        if (!mesh || !shader)
        {
            continue;
        }

        renderer.submit(
            mesh.get(),
            shader.get(),
            &entity->getMaterial(),
            entity->getTransform().getModelMatrix()
        );
    }
}

void Scene::clear()
{
    entities.clear();
    pointLights.clear();
}