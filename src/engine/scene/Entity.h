#ifndef ENTITY_H
#define ENTITY_H
#include <memory>
#include <string>
#include "engine/renderer/Material.h"
#include "engine/renderer/Mesh.h"
#include "engine/renderer/Shader.h"
#include "engine/scene/Transform.h"

class Entity
{
    std::string name;
    Transform transform;
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Shader> shader;
    Material material;
    bool castsShadows = true;
    bool affectsSSAO = true;
public:
    Entity(std::string name, std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader, const Material& material);

    const std::string& getName() const { return name; }
    Transform& getTransform() { return transform; }
    const Transform& getTransform() const { return transform; }
    Material& getMaterial() { return material; }
    const Material& getMaterial() const { return material; }
    std::shared_ptr<Mesh> getMesh() const { return mesh; }
    std::shared_ptr<Shader> getShader() const { return shader; }
    bool getCastsShadows() const { return castsShadows; }
    bool getAffectsSSAO() const { return affectsSSAO; }

    void setMesh(std::shared_ptr<Mesh> inMesh);
    void setShader(std::shared_ptr<Shader> inShader);
    void setMaterial(const Material& inMaterial);
    void setCastsShadows(bool value) { castsShadows = value; }
    void setAffectsSSAO(bool value) { affectsSSAO = value; }
};
#endif