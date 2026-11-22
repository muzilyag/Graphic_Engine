#include "engine/resources/ResourceManager.h"
#include "engine/resources/ObjLoader.h"

ResourceManager::ResourceManager() : logger("ResourceManager")
{
}

void ResourceManager::initDefaults()
{
    if (textures.find("__default_white") == textures.end())
    {
        textures["__default_white"] = std::make_shared<Texture>(255, 255, 255, 255);
    }
    if (textures.find("__default_black") == textures.end())
    {
        textures["__default_black"] = std::make_shared<Texture>(0, 0, 0, 255);
    }
    if (textures.find("__default_normal") == textures.end())
    {
        textures["__default_normal"] = std::make_shared<Texture>(128, 128, 255, 255);
    }
}

std::shared_ptr<Shader> ResourceManager::loadShader(const std::string& name, const std::string& vertPath, const std::string& fragPath)
{
    auto it = shaders.find(name);
    if (it != shaders.end())
    {
        return it->second;
    }

    auto shader = std::make_shared<Shader>(vertPath.c_str(), fragPath.c_str());
    shaders[name] = shader;
    return shader;
}

std::shared_ptr<Shader> ResourceManager::getShader(const std::string& name) const
{
    auto it = shaders.find(name);
    if (it != shaders.end())
    {
        return it->second;
    }
    logger.warn("Shader not found: " + name);
    return nullptr;
}

void ResourceManager::reloadAllShaders()
{
    int succeeded = 0;
    int failed = 0;

    for (auto& pair : shaders)
    {
        if (pair.second && pair.second->reload())
        {
            ++succeeded;
        }
        else
        {
            ++failed;
        }
    }

    logger.info("Shaders reloaded: " + std::to_string(succeeded) + " ok, " + std::to_string(failed) + " failed.");
}

std::shared_ptr<Texture> ResourceManager::loadTexture(const std::string& name, const std::string& path)
{
    auto it = textures.find(name);
    if (it != textures.end())
    {
        return it->second;
    }

    auto texture = std::make_shared<Texture>(path);
    if (!texture->isValid())
    {
        return nullptr;
    }

    textures[name] = texture;
    return texture;
}

std::shared_ptr<Texture> ResourceManager::getTexture(const std::string& name) const
{
    auto it = textures.find(name);
    if (it != textures.end())
    {
        return it->second;
    }
    logger.warn("Texture not found: " + name);
    return nullptr;
}

std::shared_ptr<Texture> ResourceManager::getDefaultWhiteTexture() const
{
    return getTexture("__default_white");
}

std::shared_ptr<Texture> ResourceManager::getDefaultBlackTexture() const
{
    return getTexture("__default_black");
}

std::shared_ptr<Texture> ResourceManager::getDefaultNormalTexture() const
{
    return getTexture("__default_normal");
}

std::shared_ptr<Mesh> ResourceManager::loadMesh(const std::string& name, const std::string& objPath)
{
    auto it = meshes.find(name);
    if (it != meshes.end())
    {
        return it->second;
    }

    auto loaded = ObjLoader::load(objPath);
    if (!loaded)
    {
        return nullptr;
    }

    std::shared_ptr<Mesh> mesh = std::move(loaded);
    meshes[name] = mesh;
    return mesh;
}

std::shared_ptr<Mesh> ResourceManager::registerMesh(const std::string& name, std::unique_ptr<Mesh> mesh)
{
    if (!mesh)
    {
        return nullptr;
    }

    std::shared_ptr<Mesh> sharedMesh = std::move(mesh);
    meshes[name] = sharedMesh;
    return sharedMesh;
}

std::shared_ptr<Mesh> ResourceManager::getMesh(const std::string& name) const
{
    auto it = meshes.find(name);
    if (it != meshes.end())
    {
        return it->second;
    }
    logger.warn("Mesh not found: " + name);
    return nullptr;
}

void ResourceManager::clear()
{
    meshes.clear();
    textures.clear();
    shaders.clear();
}