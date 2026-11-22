#include "engine/renderer/Renderer.h"
#include "engine/platform/GLLoader.h"
#include "engine/resources/ResourceManager.h"
#include <algorithm>
#include <string>

void Renderer::init(const ResourceManager& resources)
{
    defaultDiffuse  = resources.getDefaultWhiteTexture();
    defaultSpecular = resources.getDefaultWhiteTexture();
    defaultNormal   = resources.getDefaultNormalTexture();
}

void Renderer::shutdown()
{
    queue.clear();
    lights.clear();
    defaultDiffuse.reset();
    defaultSpecular.reset();
    defaultNormal.reset();
}

void Renderer::clear(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::beginFrame(const Mat4& view, const Mat4& projection, const Vec3& cameraPos)
{
    viewMatrix = view;
    projectionMatrix = projection;
    viewPos = cameraPos;
    queue.clear();
    lights.clear();
}

void Renderer::submitLight(const PointLight& light)
{
    lights.push_back(light);
}

void Renderer::submit(const Mesh* mesh, Shader* shader, const Material* material, const Mat4& modelMatrix)
{
    if (!mesh || !shader || !material)
    {
        return;
    }

    queue.push_back({ mesh, shader, material, modelMatrix });
}

void Renderer::flush()
{
    std::sort(queue.begin(), queue.end(),
        [](const RenderCommand& a, const RenderCommand& b)
        {
            return a.shader < b.shader;
        });

    Shader* activeShader = nullptr;
    constexpr int MAX_POINT_LIGHTS = 4;

    for (const RenderCommand& cmd : queue)
    {
        if (cmd.shader != activeShader)
        {
            activeShader = cmd.shader;
            activeShader->use();
            activeShader->setMat4("view", viewMatrix);
            activeShader->setMat4("projection", projectionMatrix);
            activeShader->setVec3("viewPos", viewPos);
            activeShader->setInt("material.diffuseMap", 0);
            activeShader->setInt("material.specularMap", 1);
            activeShader->setInt("material.normalMap", 2);

            for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
            {
                std::string base = "pointLights[" + std::to_string(i) + "].";
                if (static_cast<size_t>(i) < lights.size())
                {
                    const PointLight& light = lights[i];
                    activeShader->setVec3(base + "position", light.position);
                    activeShader->setVec3(base + "color", light.color);
                    activeShader->setFloat(base + "constant", light.constant);
                    activeShader->setFloat(base + "linear", light.linear);
                    activeShader->setFloat(base + "quadratic", light.quadratic);
                }
                else
                {
                    activeShader->setVec3(base + "position", {0.0f, 0.0f, 0.0f});
                    activeShader->setVec3(base + "color", {0.0f, 0.0f, 0.0f});
                    activeShader->setFloat(base + "constant", 1.0f);
                    activeShader->setFloat(base + "linear", 0.0f);
                    activeShader->setFloat(base + "quadratic", 0.0f);
                }
            }
        }

        const Material& mat = *cmd.material;
        const Texture* diffTex = mat.diffuseMap ? mat.diffuseMap.get() : defaultDiffuse.get();
        const Texture* specTex = mat.specularMap ? mat.specularMap.get() : defaultSpecular.get();
        const Texture* normTex = mat.normalMap ? mat.normalMap.get() : defaultNormal.get();

        if (diffTex)
        {
            diffTex->bind(0);
        }
        if (specTex)
        {
            specTex->bind(1);
        }
        if (normTex)
        {
            normTex->bind(2);
        }

        activeShader->setMaterial("material", mat);
        activeShader->setMat4("model", cmd.modelMatrix);
        cmd.mesh->draw();
    }

    queue.clear();
    lights.clear();
}