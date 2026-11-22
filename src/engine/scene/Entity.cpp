#include "engine/scene/Entity.h"
#include <utility>

Entity::Entity(std::string name, std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader, const Material& material)
    : name(std::move(name)), mesh(std::move(mesh)), shader(std::move(shader)), material(material)
{
}

void Entity::setMesh(std::shared_ptr<Mesh> inMesh)
{
    mesh = std::move(inMesh);
}

void Entity::setShader(std::shared_ptr<Shader> inShader)
{
    shader = std::move(inShader);
}

void Entity::setMaterial(const Material& inMaterial)
{
    material = inMaterial;
}