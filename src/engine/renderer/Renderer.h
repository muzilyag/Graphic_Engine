#ifndef RENDERER_H
#define RENDERER_H
#include <memory>
#include <vector>
#include "engine/math/Math3d.h"
#include "engine/renderer/Material.h"
#include "engine/renderer/Mesh.h"
#include "engine/renderer/Shader.h"
#include "engine/renderer/Texture.h"

class ResourceManager;

struct RenderCommand
{
    const Mesh* mesh = nullptr;
    Shader* shader = nullptr;
    const Material* material = nullptr;
    Mat4 modelMatrix;
};

class Renderer
{
    Mat4 viewMatrix;
    Mat4 projectionMatrix;
    Vec3 viewPos{0.0f, 0.0f, 0.0f};

    std::vector<RenderCommand> queue;
    std::vector<PointLight> lights;

    std::shared_ptr<Texture> defaultDiffuse;
    std::shared_ptr<Texture> defaultSpecular;
    std::shared_ptr<Texture> defaultNormal;
public:
    Renderer() = default;
    ~Renderer() = default;

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void init(const ResourceManager& resources);
    void shutdown();

    void clear(float r, float g, float b, float a = 1.0f);
    void beginFrame(const Mat4& view, const Mat4& projection, const Vec3& cameraPos);
    void submitLight(const PointLight& light);
    void submit(const Mesh* mesh, Shader* shader, const Material* material, const Mat4& modelMatrix);
    void flush();
};
#endif