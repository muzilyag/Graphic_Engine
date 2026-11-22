#ifndef RENDERER_H
#define RENDERER_H
#include <memory>
#include <vector>
#include "engine/math/Bounds.h"
#include "engine/math/Math3d.h"
#include "engine/renderer/Framebuffer.h"
#include "engine/renderer/Material.h"
#include "engine/renderer/Mesh.h"
#include "engine/renderer/Shader.h"
#include "engine/renderer/Texture.h"

class ResourceManager;

struct RenderStats
{
    int totalObjects = 0;
    int visibleObjects = 0;
    int culledObjects = 0;
    int drawCalls = 0;
};

struct RenderCommand
{
    const Mesh* mesh = nullptr;
    Shader* shader = nullptr;
    const Material* material = nullptr;
    Mat4 modelMatrix;
    AABB worldBounds;
    bool castsShadows = true;
    float distanceSq = 0.0f;
};

struct InstancedRenderCommand
{
    Mesh* mesh = nullptr;
    Shader* shader = nullptr;
    const Material* material = nullptr;
    const std::vector<Mat4>* transforms = nullptr;
    bool castsShadows = true;
};

class Renderer
{
    Mat4 viewMatrix;
    Mat4 projectionMatrix;
    Frustum cameraFrustum;
    Vec3 viewPos{0.0f, 0.0f, 0.0f};
    Vec3 sunDirection{0.45f, 0.65f, -0.6f};
    int viewportWidth = 800;
    int viewportHeight = 600;
    float clearR = 0.02f;
    float clearG = 0.02f;
    float clearB = 0.04f;
    float clearA = 1.0f;
    bool wireframe = false;
    bool bloomEnabled = true;
    bool fxaaEnabled = true;
    bool frustumCullingEnabled = true;
    float exposure = 1.0f;
    float bloomStrength = 0.4f;
    unsigned int screenVAO = 0;

    RenderStats stats;
    std::vector<RenderCommand> opaqueQueue;
    std::vector<RenderCommand> transparentQueue;
    std::vector<RenderCommand> shadowQueue;
    std::vector<InstancedRenderCommand> instancedQueue;
    std::vector<Mat4> instanceScratch;
    std::vector<PointLight> lights;
    DirectionalLight dirLight;

    std::shared_ptr<Texture> defaultWhite;
    std::shared_ptr<Texture> defaultNormal;

    std::shared_ptr<Mesh> skyboxMesh;
    std::shared_ptr<Shader> skyboxShader;

    std::unique_ptr<Framebuffer> shadowFBO;
    std::shared_ptr<Shader> shadowShader;
    std::shared_ptr<Shader> shadowInstancedShader;

    std::unique_ptr<Framebuffer> hdrFBO;
    std::unique_ptr<Framebuffer> pingPongFBO[2];
    std::unique_ptr<Framebuffer> ldrFBO;
    std::shared_ptr<Shader> blurShader;
    std::shared_ptr<Shader> postProcessShader;
    std::shared_ptr<Shader> fxaaShader;

    void bindShaderFrameData(Shader* shader, const Mat4& lightSpaceMatrix, bool& hasAlbedoMap, bool& hasNormalMap, bool& hasMetallicMap, bool& hasRoughnessMap, bool& hasAoMap);
    void bindMaterialTextures(const Material& mat, bool hasAlbedoMap, bool hasNormalMap, bool hasMetallicMap, bool hasRoughnessMap, bool hasAoMap);
    void drawCommandList(const std::vector<RenderCommand>& list, const Mat4& lightSpaceMatrix);
    void drawInstancedList(const Mat4& lightSpaceMatrix);
public:
    Renderer() = default;
    ~Renderer() = default;

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void init(const ResourceManager& resources);
    void shutdown();

    void setSkybox(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader);
    void setShadowShader(std::shared_ptr<Shader> shader, std::shared_ptr<Shader> instancedShader = nullptr);
    void setPostProcessShaders(std::shared_ptr<Shader> blur, std::shared_ptr<Shader> postProcess, std::shared_ptr<Shader> fxaa = nullptr);
    void setSunDirection(const Vec3& dir);
    void setClearColor(float r, float g, float b, float a = 1.0f);
    void setWireframe(bool enabled) { wireframe = enabled; }
    void setBloomEnabled(bool enabled) { bloomEnabled = enabled; }
    bool isBloomEnabled() const { return bloomEnabled; }
    void setFxaaEnabled(bool enabled) { fxaaEnabled = enabled; }
    bool isFxaaEnabled() const { return fxaaEnabled; }
    void setFrustumCullingEnabled(bool enabled) { frustumCullingEnabled = enabled; }
    bool isFrustumCullingEnabled() const { return frustumCullingEnabled; }
    void setExposure(float value) { exposure = value; }
    void setBloomStrength(float value) { bloomStrength = value; }
    const RenderStats& getStats() const { return stats; }

    void beginFrame(const Mat4& view, const Mat4& projection, const Vec3& cameraPos, int width, int height);
    void submitDirectionalLight(const DirectionalLight& light);
    void submitLight(const PointLight& light);
    void submit(const Mesh* mesh, Shader* shader, const Material* material, const Mat4& modelMatrix, bool castsShadows = true);
    void submitInstanced(Mesh* mesh, Shader* shader, const Material* material, const std::vector<Mat4>* transforms, bool castsShadows = true);
    void flush();
};
#endif