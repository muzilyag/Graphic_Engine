#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H
#include <memory>
#include <string>
#include <unordered_map>
#include "engine/renderer/Mesh.h"
#include "engine/renderer/Shader.h"
#include "engine/renderer/Texture.h"
#include "engine/utils/Logger.h"

class ResourceManager
{
    Logger logger;
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
    std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes;
public:
    ResourceManager();
    ~ResourceManager() = default;

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    void initDefaults();

    std::shared_ptr<Shader> loadShader(const std::string& name, const std::string& vertPath, const std::string& fragPath);
    std::shared_ptr<Shader> getShader(const std::string& name) const;

    std::shared_ptr<Texture> loadTexture(const std::string& name, const std::string& path);
    std::shared_ptr<Texture> getTexture(const std::string& name) const;
    std::shared_ptr<Texture> getDefaultWhiteTexture() const;
    std::shared_ptr<Texture> getDefaultBlackTexture() const;
    std::shared_ptr<Texture> getDefaultNormalTexture() const;

    std::shared_ptr<Mesh> loadMesh(const std::string& name, const std::string& objPath);
    std::shared_ptr<Mesh> registerMesh(const std::string& name, std::unique_ptr<Mesh> mesh);
    std::shared_ptr<Mesh> getMesh(const std::string& name) const;

    void clear();
};
#endif