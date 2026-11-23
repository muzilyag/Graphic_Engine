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

InstancedBatch& Scene::createInstancedBatch(const std::string& name, std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader, const Material& material, bool castsShadows)
{
    auto batch = std::make_unique<InstancedBatch>();
    batch->name = name;
    batch->mesh = std::move(mesh);
    batch->shader = std::move(shader);
    batch->material = material;
    batch->castsShadows = castsShadows;
    InstancedBatch& ref = *batch;
    instancedBatches.push_back(std::move(batch));
    return ref;
}

InstancedBatch* Scene::findInstancedBatch(const std::string& name)
{
    for (auto& batch : instancedBatches)
    {
        if (batch->name == name)
        {
            return batch.get();
        }
    }
    return nullptr;
}

void Scene::addPointLight(const PointLight& light)
{
    pointLights.push_back(light);
}

void Scene::setDirectionalLight(const DirectionalLight& light)
{
    directionalLight = light;
}

void Scene::setSpotLight(const SpotLight& light)
{
    spotLight = light;
}

void Scene::submit(Renderer& renderer) const
{
    renderer.submitDirectionalLight(directionalLight);
    renderer.submitSpotLight(spotLight);

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
            entity->getTransform().getModelMatrix(),
            entity->getCastsShadows(),
            entity->getAffectsSSAO()
        );
    }

    for (const auto& batch : instancedBatches)
    {
        if (!batch->mesh || !batch->shader || batch->transforms.empty())
        {
            continue;
        }

        renderer.submitInstanced(
            batch->mesh.get(),
            batch->shader.get(),
            &batch->material,
            &batch->transforms,
            batch->castsShadows
        );
    }
}

void Scene::clear()
{
    entities.clear();
    instancedBatches.clear();
    pointLights.clear();
    directionalLight = DirectionalLight{};
    spotLight = SpotLight{};
}