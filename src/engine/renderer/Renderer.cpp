#include "engine/renderer/Renderer.h"
#include "engine/platform/GLLoader.h"
#include "engine/resources/ResourceManager.h"
#include <algorithm>
#include <cmath>
#include <string>
#include <utility>

void Renderer::init(const ResourceManager& resources)
{
    defaultWhite   = resources.getDefaultWhiteTexture();
    defaultNormal  = resources.getDefaultNormalTexture();
    shadowFBO      = std::make_unique<Framebuffer>(2048, 2048, FramebufferType::DEPTH_ONLY);
    hdrFBO         = std::make_unique<Framebuffer>(viewportWidth, viewportHeight, FramebufferType::HDR_MRT);
    pingPongFBO[0] = std::make_unique<Framebuffer>(std::max(1, viewportWidth / 2), std::max(1, viewportHeight / 2), FramebufferType::HDR_COLOR);
    pingPongFBO[1] = std::make_unique<Framebuffer>(std::max(1, viewportWidth / 2), std::max(1, viewportHeight / 2), FramebufferType::HDR_COLOR);
    ldrFBO         = std::make_unique<Framebuffer>(viewportWidth, viewportHeight, FramebufferType::LDR_COLOR);
    glGenVertexArrays(1, &screenVAO);
}

void Renderer::shutdown()
{
    opaqueQueue.clear();
    transparentQueue.clear();
    shadowQueue.clear();
    instancedQueue.clear();
    instanceScratch.clear();
    lights.clear();
    if (screenVAO != 0)
    {
        glDeleteVertexArrays(1, &screenVAO);
        screenVAO = 0;
    }
    shadowFBO.reset();
    shadowShader.reset();
    shadowInstancedShader.reset();
    hdrFBO.reset();
    pingPongFBO[0].reset();
    pingPongFBO[1].reset();
    ldrFBO.reset();
    blurShader.reset();
    postProcessShader.reset();
    fxaaShader.reset();
    defaultWhite.reset();
    defaultNormal.reset();
    skyboxMesh.reset();
    skyboxShader.reset();
}

void Renderer::setSkybox(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader)
{
    skyboxMesh = std::move(mesh);
    skyboxShader = std::move(shader);
}

void Renderer::setShadowShader(std::shared_ptr<Shader> shader, std::shared_ptr<Shader> instancedShader)
{
    shadowShader = std::move(shader);
    shadowInstancedShader = std::move(instancedShader);
}

void Renderer::setPostProcessShaders(std::shared_ptr<Shader> blur, std::shared_ptr<Shader> postProcess, std::shared_ptr<Shader> fxaa)
{
    blurShader = std::move(blur);
    postProcessShader = std::move(postProcess);
    fxaaShader = std::move(fxaa);
}

void Renderer::setSunDirection(const Vec3& dir)
{
    sunDirection = Vec3::normalize(dir);
}

void Renderer::setClearColor(float r, float g, float b, float a)
{
    clearR = r;
    clearG = g;
    clearB = b;
    clearA = a;
}

void Renderer::beginFrame(const Mat4& view, const Mat4& projection, const Vec3& cameraPos, int width, int height)
{
    viewMatrix = view;
    projectionMatrix = projection;
    cameraFrustum = Frustum::fromViewProjection(Mat4::multiply(projectionMatrix, viewMatrix));
    viewPos = cameraPos;
    viewportWidth = width;
    viewportHeight = height;

    if (hdrFBO && (hdrFBO->getWidth() != width || hdrFBO->getHeight() != height))
    {
        hdrFBO->resize(width, height);
    }
    if (ldrFBO && (ldrFBO->getWidth() != width || ldrFBO->getHeight() != height))
    {
        ldrFBO->resize(width, height);
    }
    int halfW = std::max(1, width / 2);
    int halfH = std::max(1, height / 2);
    if (pingPongFBO[0] && (pingPongFBO[0]->getWidth() != halfW || pingPongFBO[0]->getHeight() != halfH))
    {
        pingPongFBO[0]->resize(halfW, halfH);
    }
    if (pingPongFBO[1] && (pingPongFBO[1]->getWidth() != halfW || pingPongFBO[1]->getHeight() != halfH))
    {
        pingPongFBO[1]->resize(halfW, halfH);
    }

    stats = RenderStats{};
    opaqueQueue.clear();
    transparentQueue.clear();
    shadowQueue.clear();
    instancedQueue.clear();
    lights.clear();
    dirLight = DirectionalLight{};
}

void Renderer::submitDirectionalLight(const DirectionalLight& light)
{
    dirLight = light;
    if (dirLight.enabled)
    {
        Vec3 normDir = Vec3::normalize(dirLight.direction);
        if (Vec3::dot(normDir, normDir) > 1e-6f)
        {
            dirLight.direction = normDir;
            sunDirection = normDir * -1.0f;
        }
    }
}

void Renderer::submitLight(const PointLight& light)
{
    lights.push_back(light);
}

void Renderer::submit(const Mesh* mesh, Shader* shader, const Material* material, const Mat4& modelMatrix, bool castsShadows)
{
    if (!mesh || !shader || !material)
    {
        return;
    }

    ++stats.totalObjects;
    AABB worldBounds = mesh->getBounds().transformed(modelMatrix);
    bool isTransparent = material->transparent || material->opacity < 0.999f;

    if (!isTransparent && castsShadows)
    {
        shadowQueue.push_back({ mesh, nullptr, nullptr, modelMatrix, worldBounds, true, 0.0f });
    }

    if (frustumCullingEnabled && !cameraFrustum.intersects(worldBounds))
    {
        ++stats.culledObjects;
        return;
    }

    ++stats.visibleObjects;

    if (isTransparent)
    {
        Vec3 pos = { modelMatrix.m[12], modelMatrix.m[13], modelMatrix.m[14] };
        Vec3 delta = pos - viewPos;
        float distSq = Vec3::dot(delta, delta);
        transparentQueue.push_back({ mesh, shader, material, modelMatrix, worldBounds, false, distSq });
    }
    else
    {
        opaqueQueue.push_back({ mesh, shader, material, modelMatrix, worldBounds, castsShadows, 0.0f });
    }
}

void Renderer::submitInstanced(Mesh* mesh, Shader* shader, const Material* material, const std::vector<Mat4>* transforms, bool castsShadows)
{
    if (!mesh || !shader || !material || !transforms || transforms->empty())
    {
        return;
    }

    stats.totalObjects += static_cast<int>(transforms->size());
    instancedQueue.push_back({ mesh, shader, material, transforms, castsShadows });
}

void Renderer::bindShaderFrameData(Shader* shader, const Mat4& lightSpaceMatrix, bool& hasAlbedoMap, bool& hasNormalMap, bool& hasMetallicMap, bool& hasRoughnessMap, bool& hasAoMap)
{
    constexpr int MAX_POINT_LIGHTS = 8;

    shader->use();

    if (shader->hasUniform("view"))
    {
        shader->setMat4("view", viewMatrix);
    }
    if (shader->hasUniform("projection"))
    {
        shader->setMat4("projection", projectionMatrix);
    }
    if (shader->hasUniform("viewPos"))
    {
        shader->setVec3("viewPos", viewPos);
    }
    if (shader->hasUniform("sunDirection"))
    {
        shader->setVec3("sunDirection", sunDirection);
    }
    if (shader->hasUniform("lightSpaceMatrix"))
    {
        shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    }

    hasAlbedoMap    = shader->hasUniform("material.albedoMap");
    hasNormalMap    = shader->hasUniform("material.normalMap");
    hasMetallicMap  = shader->hasUniform("material.metallicMap");
    hasRoughnessMap = shader->hasUniform("material.roughnessMap");
    hasAoMap        = shader->hasUniform("material.aoMap");

    if (hasAlbedoMap)
    {
        shader->setInt("material.albedoMap", 0);
    }
    if (hasNormalMap)
    {
        shader->setInt("material.normalMap", 1);
    }
    if (hasMetallicMap)
    {
        shader->setInt("material.metallicMap", 2);
    }
    if (hasRoughnessMap)
    {
        shader->setInt("material.roughnessMap", 3);
    }
    if (hasAoMap)
    {
        shader->setInt("material.aoMap", 4);
    }
    if (shader->hasUniform("shadowMap"))
    {
        if (shadowFBO && shadowFBO->isValid())
        {
            shadowFBO->bindDepthTexture(5);
        }
        shader->setInt("shadowMap", 5);
    }

    if (shader->hasUniform("hasDirLight"))
    {
        shader->setInt("hasDirLight", dirLight.enabled ? 1 : 0);
        if (dirLight.enabled)
        {
            shader->setVec3("dirLight.direction", dirLight.direction);
            shader->setVec3("dirLight.color", dirLight.color);
            shader->setFloat("dirLight.intensity", dirLight.intensity);
        }
    }

    if (shader->hasUniform("activePointLights"))
    {
        int activeCount = std::min(static_cast<int>(lights.size()), MAX_POINT_LIGHTS);
        shader->setInt("activePointLights", activeCount);

        for (int i = 0; i < activeCount; ++i)
        {
            const PointLight& light = lights[i];
            std::string base = "pointLights[" + std::to_string(i) + "].";
            shader->setVec3(base + "position", light.position);
            shader->setVec3(base + "color", light.color);
            shader->setFloat(base + "intensity", light.intensity);
            shader->setFloat(base + "radius", light.radius);
        }
    }
}

void Renderer::bindMaterialTextures(const Material& mat, bool hasAlbedoMap, bool hasNormalMap, bool hasMetallicMap, bool hasRoughnessMap, bool hasAoMap)
{
    if (hasAlbedoMap)
    {
        const Texture* tex = mat.albedoMap ? mat.albedoMap.get() : defaultWhite.get();
        if (tex)
        {
            tex->bind(0);
        }
    }
    if (hasNormalMap)
    {
        const Texture* tex = mat.normalMap ? mat.normalMap.get() : defaultNormal.get();
        if (tex)
        {
            tex->bind(1);
        }
    }
    if (hasMetallicMap)
    {
        const Texture* tex = mat.metallicMap ? mat.metallicMap.get() : defaultWhite.get();
        if (tex)
        {
            tex->bind(2);
        }
    }
    if (hasRoughnessMap)
    {
        const Texture* tex = mat.roughnessMap ? mat.roughnessMap.get() : defaultWhite.get();
        if (tex)
        {
            tex->bind(3);
        }
    }
    if (hasAoMap)
    {
        const Texture* tex = mat.aoMap ? mat.aoMap.get() : defaultWhite.get();
        if (tex)
        {
            tex->bind(4);
        }
    }
}

void Renderer::drawCommandList(const std::vector<RenderCommand>& list, const Mat4& lightSpaceMatrix)
{
    Shader* activeShader = nullptr;
    bool hasAlbedoMap = false;
    bool hasNormalMap = false;
    bool hasMetallicMap = false;
    bool hasRoughnessMap = false;
    bool hasAoMap = false;
    bool cullEnabled = true;

    for (const RenderCommand& cmd : list)
    {
        if (cmd.shader != activeShader)
        {
            activeShader = cmd.shader;
            bindShaderFrameData(activeShader, lightSpaceMatrix, hasAlbedoMap, hasNormalMap, hasMetallicMap, hasRoughnessMap, hasAoMap);
        }

        const Material& mat = *cmd.material;
        bool wantCull = !mat.doubleSided;
        if (wantCull != cullEnabled)
        {
            if (wantCull)
            {
                glEnable(GL_CULL_FACE);
            }
            else
            {
                glDisable(GL_CULL_FACE);
            }
            cullEnabled = wantCull;
        }

        bindMaterialTextures(mat, hasAlbedoMap, hasNormalMap, hasMetallicMap, hasRoughnessMap, hasAoMap);
        activeShader->setMaterial("material", mat);
        activeShader->setMat4("model", cmd.modelMatrix);
        cmd.mesh->draw();
        ++stats.drawCalls;
    }

    if (!cullEnabled)
    {
        glEnable(GL_CULL_FACE);
    }
}

void Renderer::drawInstancedList(const Mat4& lightSpaceMatrix)
{
    if (instancedQueue.empty())
    {
        return;
    }

    std::sort(instancedQueue.begin(), instancedQueue.end(),
        [](const InstancedRenderCommand& a, const InstancedRenderCommand& b)
        {
            return a.shader < b.shader;
        });

    Shader* activeShader = nullptr;
    bool hasAlbedoMap = false;
    bool hasNormalMap = false;
    bool hasMetallicMap = false;
    bool hasRoughnessMap = false;
    bool hasAoMap = false;
    bool cullEnabled = true;

    for (const InstancedRenderCommand& batch : instancedQueue)
    {
        const std::vector<Mat4>* drawTransforms = batch.transforms;

        if (frustumCullingEnabled)
        {
            instanceScratch.clear();
            const AABB& localBounds = batch.mesh->getBounds();
            for (const Mat4& mat : *batch.transforms)
            {
                if (cameraFrustum.intersects(localBounds.transformed(mat)))
                {
                    instanceScratch.push_back(mat);
                }
            }

            int visibleCount = static_cast<int>(instanceScratch.size());
            stats.visibleObjects += visibleCount;
            stats.culledObjects += static_cast<int>(batch.transforms->size()) - visibleCount;

            if (visibleCount == 0)
            {
                continue;
            }
            drawTransforms = &instanceScratch;
        }
        else
        {
            stats.visibleObjects += static_cast<int>(batch.transforms->size());
        }

        if (batch.shader != activeShader)
        {
            activeShader = batch.shader;
            bindShaderFrameData(activeShader, lightSpaceMatrix, hasAlbedoMap, hasNormalMap, hasMetallicMap, hasRoughnessMap, hasAoMap);
        }

        const Material& mat = *batch.material;
        bool wantCull = !mat.doubleSided;
        if (wantCull != cullEnabled)
        {
            if (wantCull)
            {
                glEnable(GL_CULL_FACE);
            }
            else
            {
                glDisable(GL_CULL_FACE);
            }
            cullEnabled = wantCull;
        }

        bindMaterialTextures(mat, hasAlbedoMap, hasNormalMap, hasMetallicMap, hasRoughnessMap, hasAoMap);
        activeShader->setMaterial("material", mat);
        batch.mesh->drawInstanced(*drawTransforms);
        ++stats.drawCalls;
    }

    if (!cullEnabled)
    {
        glEnable(GL_CULL_FACE);
    }
}

void Renderer::flush()
{
    Mat4 lightSpaceMatrix = Mat4::identity();
    bool useShadows = dirLight.enabled && shadowFBO && shadowFBO->isValid() && shadowShader != nullptr;

    if (useShadows)
    {
        Vec3 lightDir = Vec3::normalize(dirLight.direction);
        Vec3 sceneCenter = { 0.0f, -1.5f, -4.0f };
        Vec3 lightPos = sceneCenter - lightDir * 22.0f;
        Vec3 up = (fabsf(lightDir.y) > 0.99f) ? Vec3{ 0.0f, 0.0f, 1.0f } : Vec3{ 0.0f, 1.0f, 0.0f };

        Mat4 lightView = Mat4::lookAt(lightPos, sceneCenter, up);
        Mat4 lightProj = Mat4::ortho(-16.0f, 16.0f, -16.0f, 16.0f, 1.0f, 55.0f);
        lightSpaceMatrix = Mat4::multiply(lightProj, lightView);
        Frustum lightFrustum = Frustum::fromViewProjection(lightSpaceMatrix);

        shadowFBO->bind();
        glViewport(0, 0, shadowFBO->getWidth(), shadowFBO->getHeight());
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.5f, 3.0f);

        shadowShader->use();
        shadowShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

        for (const RenderCommand& cmd : shadowQueue)
        {
            if (frustumCullingEnabled && !lightFrustum.intersects(cmd.worldBounds))
            {
                continue;
            }
            shadowShader->setMat4("model", cmd.modelMatrix);
            cmd.mesh->draw();
            ++stats.drawCalls;
        }

        if (shadowInstancedShader && !instancedQueue.empty())
        {
            shadowInstancedShader->use();
            shadowInstancedShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

            for (const InstancedRenderCommand& batch : instancedQueue)
            {
                if (!batch.castsShadows)
                {
                    continue;
                }

                instanceScratch.clear();
                const AABB& localBounds = batch.mesh->getBounds();
                for (const Mat4& mat : *batch.transforms)
                {
                    if (!frustumCullingEnabled || lightFrustum.intersects(localBounds.transformed(mat)))
                    {
                        instanceScratch.push_back(mat);
                    }
                }

                if (!instanceScratch.empty())
                {
                    batch.mesh->drawInstanced(instanceScratch);
                    ++stats.drawCalls;
                }
            }
        }

        glDisable(GL_POLYGON_OFFSET_FILL);
        shadowFBO->unbind();
    }

    bool useHDR = hdrFBO && hdrFBO->isValid() && postProcessShader != nullptr && screenVAO != 0;
    if (useHDR)
    {
        hdrFBO->bind();
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glViewport(0, 0, viewportWidth, viewportHeight);
    glClearColor(clearR, clearG, clearB, clearA);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

    std::sort(opaqueQueue.begin(), opaqueQueue.end(),
        [](const RenderCommand& a, const RenderCommand& b)
        {
            return a.shader < b.shader;
        });

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    drawCommandList(opaqueQueue, lightSpaceMatrix);
    drawInstancedList(lightSpaceMatrix);

    if (skyboxMesh && skyboxShader)
    {
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        skyboxShader->use();
        skyboxShader->setMat4("view", viewMatrix);
        skyboxShader->setMat4("projection", projectionMatrix);
        if (skyboxShader->hasUniform("sunDirection"))
        {
            skyboxShader->setVec3("sunDirection", sunDirection);
        }
        skyboxMesh->draw();
        ++stats.drawCalls;

        glCullFace(GL_BACK);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }

    if (!transparentQueue.empty())
    {
        std::sort(transparentQueue.begin(), transparentQueue.end(),
            [](const RenderCommand& a, const RenderCommand& b)
            {
                return a.distanceSq > b.distanceSq;
            });

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        drawCommandList(transparentQueue, lightSpaceMatrix);

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

    if (useHDR)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glBindVertexArray(screenVAO);

        bool doBloom = bloomEnabled && !wireframe && blurShader != nullptr
                    && pingPongFBO[0] && pingPongFBO[0]->isValid()
                    && pingPongFBO[1] && pingPongFBO[1]->isValid();

        bool horizontal = true;
        if (doBloom)
        {
            glViewport(0, 0, pingPongFBO[0]->getWidth(), pingPongFBO[0]->getHeight());
            blurShader->use();
            blurShader->setInt("image", 0);

            bool firstIteration = true;
            constexpr int BLUR_PASSES = 6;
            for (int i = 0; i < BLUR_PASSES; ++i)
            {
                int targetIdx = horizontal ? 1 : 0;
                int sourceIdx = horizontal ? 0 : 1;

                pingPongFBO[targetIdx]->bind();
                blurShader->setInt("horizontal", horizontal ? 1 : 0);

                if (firstIteration)
                {
                    hdrFBO->bindColorTexture(1, 0);
                    firstIteration = false;
                }
                else
                {
                    pingPongFBO[sourceIdx]->bindColorTexture(0, 0);
                }

                glDrawArrays(GL_TRIANGLES, 0, 3);
                horizontal = !horizontal;
            }
        }

        bool doFXAA = fxaaEnabled && !wireframe && fxaaShader != nullptr && ldrFBO && ldrFBO->isValid();

        if (doFXAA)
        {
            ldrFBO->bind();
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        glViewport(0, 0, viewportWidth, viewportHeight);
        glClear(GL_COLOR_BUFFER_BIT);

        postProcessShader->use();
        hdrFBO->bindColorTexture(0, 0);
        postProcessShader->setInt("sceneTexture", 0);

        if (doBloom)
        {
            int finalBloomIdx = horizontal ? 0 : 1;
            pingPongFBO[finalBloomIdx]->bindColorTexture(0, 1);
            postProcessShader->setInt("bloomTexture", 1);
            postProcessShader->setInt("useBloom", 1);
            postProcessShader->setFloat("bloomStrength", bloomStrength);
        }
        else
        {
            postProcessShader->setInt("useBloom", 0);
        }

        postProcessShader->setFloat("exposure", exposure);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        if (doFXAA)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, viewportWidth, viewportHeight);
            glClear(GL_COLOR_BUFFER_BIT);

            fxaaShader->use();
            ldrFBO->bindColorTexture(0, 0);
            fxaaShader->setInt("screenTexture", 0);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        glBindVertexArray(0);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
    }

    opaqueQueue.clear();
    transparentQueue.clear();
    shadowQueue.clear();
    instancedQueue.clear();
    lights.clear();
}