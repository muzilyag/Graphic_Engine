#include "sandbox/SandboxApp.h"
#include "engine/renderer/Primitives.h"
#include <cmath>
#include <random>

namespace
{
    void initOrbitData(std::vector<OrbitInstanceData>& outData, int count, std::mt19937& rng, float minRadius, float maxRadius, float heightOffset, float scaleMul, float speedSign)
    {
        std::uniform_real_distribution<float> angleDist(0.0f, 6.2831853f);
        std::uniform_real_distribution<float> radiusDist(minRadius, maxRadius);
        std::uniform_real_distribution<float> heightDist(-3.0f, 24.0f);
        std::uniform_real_distribution<float> speedDist(0.04f, 0.22f);
        std::uniform_real_distribution<float> rotDist(-1.8f, 1.8f);
        std::uniform_real_distribution<float> scaleDist(0.25f, 0.95f);
        std::uniform_real_distribution<float> bobAmpDist(0.2f, 1.2f);
        std::uniform_real_distribution<float> bobSpeedDist(0.4f, 1.4f);

        outData.clear();
        outData.reserve(count);
        for (int i = 0; i < count; ++i)
        {
            OrbitInstanceData d;
            d.radius     = radiusDist(rng);
            d.baseAngle  = angleDist(rng);
            d.orbitSpeed = speedDist(rng) * ((i % 2 == 0) ? speedSign : -speedSign);
            d.height     = heightDist(rng) + heightOffset;
            d.bobAmp     = bobAmpDist(rng);
            d.bobSpeed   = bobSpeedDist(rng);
            d.rotSpeedY  = rotDist(rng);
            d.rotSpeedX  = rotDist(rng);
            d.scale      = scaleDist(rng) * scaleMul;
            outData.push_back(d);
        }
    }
}

SandboxApp::SandboxApp() : Application(800, 600, "Engine")
{
}

void SandboxApp::onInit()
{
    auto cubeMesh       = resources.registerMesh("cube", Primitives::createCube());
    auto planeMesh      = resources.registerMesh("plane", Primitives::createPlane(20.0f, 8.0f));
    auto glassMesh      = resources.registerMesh("glass_panel", Primitives::createPlane(1.8f, 1.0f));
    auto sphereMesh     = resources.registerMesh("sphere", Primitives::createSphere(0.7f, 32, 16));
    auto smallSphere    = resources.registerMesh("small_sphere", Primitives::createSphere(0.45f, 32, 16));
    auto instSphereMesh = resources.registerMesh("inst_sphere", Primitives::createSphere(0.32f, 16, 10));
    auto pyramidMesh    = resources.loadMesh("pyramid", "./assets/models/pyramid.obj");

    auto defaultShader          = resources.loadShader("default", "./assets/shaders/shader.vert", "./assets/shaders/shader.frag");
    auto defaultInstShader      = resources.loadShader("default_instanced", "./assets/shaders/shader_instanced.vert", "./assets/shaders/shader.frag");
    auto unlitShader            = resources.loadShader("unlit", "./assets/shaders/unlit.vert", "./assets/shaders/unlit.frag");
    auto skyboxShader           = resources.loadShader("skybox", "./assets/shaders/skybox.vert", "./assets/shaders/skybox.frag");
    auto shadowShader           = resources.loadShader("shadow", "./assets/shaders/shadow.vert", "./assets/shaders/shadow.frag");
    auto shadowInstancedShader  = resources.loadShader("shadow_instanced", "./assets/shaders/shadow_instanced.vert", "./assets/shaders/shadow.frag");
    auto gBufferShader          = resources.loadShader("gbuffer", "./assets/shaders/gbuffer.vert", "./assets/shaders/gbuffer.frag");
    auto gBufferInstancedShader = resources.loadShader("gbuffer_instanced", "./assets/shaders/gbuffer_instanced.vert", "./assets/shaders/gbuffer.frag");
    auto ssaoShader             = resources.loadShader("ssao", "./assets/shaders/fullscreen.vert", "./assets/shaders/ssao.frag");
    auto ssaoBlurShader         = resources.loadShader("ssao_blur", "./assets/shaders/fullscreen.vert", "./assets/shaders/ssao_blur.frag");
    auto blurShader             = resources.loadShader("blur", "./assets/shaders/fullscreen.vert", "./assets/shaders/blur.frag");
    auto postProcessShader      = resources.loadShader("postprocess", "./assets/shaders/fullscreen.vert", "./assets/shaders/postprocess.frag");
    auto fxaaShader             = resources.loadShader("fxaa", "./assets/shaders/fullscreen.vert", "./assets/shaders/fxaa.frag");

    renderer.setSkybox(cubeMesh, skyboxShader);
    renderer.setShadowShader(shadowShader, shadowInstancedShader);
    renderer.setSSAOShaders(gBufferShader, gBufferInstancedShader, ssaoShader, ssaoBlurShader);
    renderer.setPostProcessShaders(blurShader, postProcessShader, fxaaShader);
    renderer.setExposure(1.0f);
    renderer.setBloomStrength(0.45f);
    renderer.setBloomThreshold(1.2f);

    DirectionalLight sun;
    sun.direction = {-0.45f, -0.65f, 0.6f};
    sun.color     = {1.0f, 0.95f, 0.88f};
    sun.intensity = 2.6f;
    sun.enabled   = true;
    scene.setDirectionalLight(sun);

    SpotLight flashlight;
    flashlight.position    = camera.getPos();
    flashlight.direction   = camera.getFront();
    flashlight.color       = {1.0f, 0.95f, 0.85f};
    flashlight.intensity   = 65.0f;
    flashlight.radius      = 35.0f;
    flashlight.innerCutOff = cosf(12.0f * 0.0174532925f);
    flashlight.outerCutOff = cosf(22.0f * 0.0174532925f);
    flashlight.enabled     = flashlightEnabled;
    scene.setSpotLight(flashlight);

    auto cubeTex    = resources.loadTexture("cube_diffuse", "./assets/textures/i1.bmp");
    auto modelTex   = resources.loadTexture("model_diffuse", "./assets/textures/tex.tga");
    auto specMap    = resources.loadTexture("specular", "./assets/textures/spec.bmp");
    auto normalMap  = resources.loadTexture("normal", "./assets/textures/kamen3.bmp");
    auto missingTex = resources.loadTexture("missing_test", "./assets/textures/does_not_exist.tga");

    Material texturedStone;
    texturedStone.albedo       = {0.95f, 0.88f, 0.78f};
    texturedStone.metallic     = 0.12f;
    texturedStone.roughness    = 0.85f;
    texturedStone.ao           = 1.0f;
    texturedStone.albedoMap    = cubeTex;
    texturedStone.roughnessMap = specMap;
    texturedStone.normalMap    = normalMap;

    Material matteRed;
    matteRed.albedo    = {0.85f, 0.14f, 0.14f};
    matteRed.metallic  = 0.0f;
    matteRed.roughness = 0.88f;

    Material glossyBlue;
    glossyBlue.albedo    = {0.14f, 0.38f, 0.92f};
    glossyBlue.metallic  = 0.0f;
    glossyBlue.roughness = 0.12f;

    Material pyramidMat;
    pyramidMat.albedo    = {1.0f, 1.0f, 1.0f};
    pyramidMat.metallic  = 0.0f;
    pyramidMat.roughness = 0.45f;
    pyramidMat.albedoMap = modelTex;

    Material polishedGold;
    polishedGold.albedo    = {1.0f, 0.78f, 0.34f};
    polishedGold.metallic  = 1.0f;
    polishedGold.roughness = 0.12f;

    Material brushedChrome;
    brushedChrome.albedo    = {0.92f, 0.93f, 0.95f};
    brushedChrome.metallic  = 1.0f;
    brushedChrome.roughness = 0.28f;
    brushedChrome.normalMap = normalMap;

    Material missingTexMat;
    missingTexMat.albedo    = {0.2f, 0.85f, 0.25f};
    missingTexMat.metallic  = 0.0f;
    missingTexMat.roughness = 0.4f;
    missingTexMat.albedoMap = missingTex;

    Material glassRippled;
    glassRippled.albedo              = {0.82f, 0.96f, 1.0f};
    glassRippled.metallic            = 0.0f;
    glassRippled.roughness           = 0.06f;
    glassRippled.opacity             = 1.0f;
    glassRippled.transmission        = 0.96f;
    glassRippled.ior                 = 1.52f;
    glassRippled.refractionStrength  = 0.45f;
    glassRippled.chromaticAberration = 0.14f;
    glassRippled.transparent         = true;
    glassRippled.doubleSided         = true;
    glassRippled.normalMap           = normalMap;

    Material glassFrosted;
    glassFrosted.albedo              = {0.94f, 0.97f, 1.0f};
    glassFrosted.metallic            = 0.0f;
    glassFrosted.roughness           = 0.38f;
    glassFrosted.opacity             = 1.0f;
    glassFrosted.transmission        = 0.95f;
    glassFrosted.ior                 = 1.48f;
    glassFrosted.refractionStrength  = 0.30f;
    glassFrosted.chromaticAberration = 0.06f;
    glassFrosted.transparent         = true;
    glassFrosted.doubleSided         = true;

    Material glassAlphaBlend;
    glassAlphaBlend.albedo      = {0.95f, 0.22f, 0.68f};
    glassAlphaBlend.metallic    = 0.05f;
    glassAlphaBlend.roughness   = 0.08f;
    glassAlphaBlend.opacity     = 0.36f;
    glassAlphaBlend.transparent = true;
    glassAlphaBlend.doubleSided = true;

    Material crystalSphereMat;
    crystalSphereMat.albedo              = {0.92f, 0.97f, 1.0f};
    crystalSphereMat.metallic            = 0.0f;
    crystalSphereMat.roughness           = 0.02f;
    crystalSphereMat.opacity             = 1.0f;
    crystalSphereMat.transmission        = 0.98f;
    crystalSphereMat.ior                 = 1.55f;
    crystalSphereMat.refractionStrength  = 0.75f;
    crystalSphereMat.chromaticAberration = 0.18f;
    crystalSphereMat.transparent         = true;
    crystalSphereMat.doubleSided         = false;

    Entity& floor = scene.createEntity("floor", planeMesh, defaultShader, texturedStone);
    floor.getTransform().setPosition({0.0f, -3.5f, -4.0f});

    sphereEntity = &scene.createEntity("sphere", sphereMesh, defaultShader, texturedStone);
    sphereEntity->getTransform().setPosition({0.0f, 1.6f, -2.0f});

    Entity& testMatte = scene.createEntity("test_matte", smallSphere, defaultShader, matteRed);
    testMatte.getTransform().setPosition({-3.2f, -3.05f, -1.5f});

    Entity& testGlossy = scene.createEntity("test_glossy", smallSphere, defaultShader, glossyBlue);
    testGlossy.getTransform().setPosition({-1.9f, -3.05f, -1.5f});

    if (pyramidMesh)
    {
        pyramidEntity = &scene.createEntity("pyramid", pyramidMesh, defaultShader, pyramidMat);
        pyramidEntity->getTransform().setPosition({-0.6f, -3.5f, -1.5f});
    }

    Entity& testGold = scene.createEntity("test_gold", smallSphere, defaultShader, polishedGold);
    testGold.getTransform().setPosition({0.7f, -3.05f, -1.5f});

    Entity& testChrome = scene.createEntity("test_chrome", smallSphere, defaultShader, brushedChrome);
    testChrome.getTransform().setPosition({2.0f, -3.05f, -1.5f});

    Entity& testMissing = scene.createEntity("test_missing_tex", smallSphere, defaultShader, missingTexMat);
    testMissing.getTransform().setPosition({3.3f, -3.05f, -1.5f});

    Quat upright = Quat::fromAxisAngle({1.0f, 0.0f, 0.0f}, 1.5707963f);

    Entity& glass1 = scene.createEntity("glass_rippled", glassMesh, defaultShader, glassRippled);
    glass1.getTransform().setPosition({-2.2f, -2.55f, 0.35f});
    glass1.getTransform().setRotation(upright);
    glass1.setCastsShadows(false);

    Entity& glass2 = scene.createEntity("glass_frosted", glassMesh, defaultShader, glassFrosted);
    glass2.getTransform().setPosition({0.0f, -2.55f, 0.35f});
    glass2.getTransform().setRotation(upright);
    glass2.setCastsShadows(false);

    Entity& glass3 = scene.createEntity("glass_alpha", glassMesh, defaultShader, glassAlphaBlend);
    glass3.getTransform().setPosition({2.2f, -2.55f, 0.35f});
    glass3.getTransform().setRotation(upright);
    glass3.setCastsShadows(false);

    Entity& glassSphere = scene.createEntity("glass_sphere", sphereMesh, defaultShader, crystalSphereMat);
    glassSphere.getTransform().setPosition({-2.2f, 0.25f, -1.2f});
    glassSphere.setCastsShadows(false);

    Vec3 cubePositions[5] = {
        { 0.0f,  0.0f,  0.0f},
        { 2.0f,  5.0f, -15.0f},
        {-1.5f, -2.2f, -2.5f},
        {-3.8f, -2.0f, -12.3f},
        { 2.4f, -0.4f, -3.5f}
    };

    for (int i = 0; i < 5; ++i)
    {
        Entity& cube = scene.createEntity("cube_" + std::to_string(i), cubeMesh, defaultShader, texturedStone);
        cube.getTransform().setPosition(cubePositions[i]);
        cubeEntities.push_back(&cube);
    }

    PointLight initialLights[4] = {
        {{ 0.7f,  0.2f,   2.0f}, {1.0f, 0.2f, 0.2f}, 22.0f, 18.0f},
        {{ 2.3f, -2.2f,  -4.0f}, {0.2f, 1.0f, 0.2f}, 22.0f, 18.0f},
        {{-4.0f,  2.0f, -12.0f}, {0.2f, 0.5f, 1.0f}, 28.0f, 22.0f},
        {{ 0.0f,  0.5f,  -3.0f}, {1.0f, 1.0f, 1.0f}, 24.0f, 20.0f}
    };

    for (int i = 0; i < 4; ++i)
    {
        scene.addPointLight(initialLights[i]);

        Material lightMat;
        lightMat.albedo = initialLights[i].color * 4.5f;
        Entity& lightCube = scene.createEntity("light_" + std::to_string(i), cubeMesh, unlitShader, lightMat);
        lightCube.getTransform().setPosition(initialLights[i].position);
        lightCube.getTransform().setScale({0.18f, 0.18f, 0.18f});
        lightCube.setCastsShadows(false);
        lightCube.setAffectsSSAO(false);
        lightEntities.push_back(&lightCube);
    }

    Material instCubeMat;
    instCubeMat.albedo    = {0.82f, 0.85f, 0.90f};
    instCubeMat.metallic  = 0.75f;
    instCubeMat.roughness = 0.28f;
    instCubeMat.albedoMap = cubeTex;
    instCubeMat.normalMap = normalMap;

    Material instSphereMat;
    instSphereMat.albedo    = {1.0f, 0.78f, 0.34f};
    instSphereMat.metallic  = 1.0f;
    instSphereMat.roughness = 0.16f;

    instancedCubes = &scene.createInstancedBatch("asteroid_cubes", cubeMesh, defaultInstShader, instCubeMat, true);
    instancedSpheres = &scene.createInstancedBatch("asteroid_spheres", instSphereMesh, defaultInstShader, instSphereMat, true);

    std::mt19937 rng(42);
    constexpr int CUBE_INSTANCES = 3500;
    constexpr int SPHERE_INSTANCES = 1500;

    initOrbitData(cubeOrbitData, CUBE_INSTANCES, rng, 10.5f, 65.0f, 0.0f, 1.0f, 1.0f);
    initOrbitData(sphereOrbitData, SPHERE_INSTANCES, rng, 12.0f, 60.0f, 1.5f, 1.1f, -1.0f);

    instancedCubes->transforms.resize(CUBE_INSTANCES);
    instancedSpheres->transforms.resize(SPHERE_INSTANCES);

    updateInstancedOrbit(instancedCubes, cubeOrbitData);
    updateInstancedOrbit(instancedSpheres, sphereOrbitData);
}

void SandboxApp::updateInstancedOrbit(InstancedBatch* batch, const std::vector<OrbitInstanceData>& data)
{
    if (!batch)
    {
        return;
    }

    constexpr float CENTER_X = 0.0f;
    constexpr float CENTER_Z = -4.0f;
    size_t count = data.size();

    for (size_t i = 0; i < count; ++i)
    {
        const OrbitInstanceData& d = data[i];
        float a = d.baseAngle + runTime * d.orbitSpeed;
        Vec3 pos = {
            CENTER_X + cosf(a) * d.radius,
            d.height + sinf(runTime * d.bobSpeed + d.baseAngle) * d.bobAmp,
            CENTER_Z + sinf(a) * d.radius
        };
        Vec3 scl = { d.scale, d.scale, d.scale };
        batch->transforms[i] = Mat4::trsEulerYX(pos, runTime * d.rotSpeedY, runTime * d.rotSpeedX, scl);
    }
}

void SandboxApp::onUpdate(float dt)
{
    (void)dt;

    if (isKeyPressed('F'))
    {
        flashlightEnabled = !flashlightEnabled;
    }

    SpotLight& spot = scene.getSpotLight();
    Vec3 camPos = camera.getPos();
    spot.position  = { camPos.x, camPos.y - 0.12f, camPos.z };
    spot.direction = camera.getFront();
    spot.enabled   = flashlightEnabled;

    if (sphereEntity)
    {
        sphereEntity->getTransform().setRotation(Quat::fromAxisAngle({0.0f, 1.0f, 0.0f}, runTime * 0.7f));
    }

    if (pyramidEntity)
    {
        pyramidEntity->getTransform().setRotation(Quat::fromAxisAngle({0.0f, 1.0f, 0.0f}, runTime));
    }

    for (size_t i = 0; i < cubeEntities.size(); ++i)
    {
        float angle = runTime * static_cast<float>(i + 1);
        cubeEntities[i]->getTransform().setRotation(Quat::fromAxisAngle({1.0f, 0.3f, 0.5f}, angle));
    }

    auto& lights = scene.getPointLights();
    if (lights.size() >= 4 && lightEntities.size() >= 4)
    {
        Vec3 pos0 = { cosf(runTime * 0.8f) * 3.5f, -1.2f, -1.5f + sinf(runTime * 0.8f) * 3.5f };
        lights[0].position = pos0;
        lightEntities[0]->getTransform().setPosition(pos0);

        Vec3 pos3 = { sinf(runTime * 1.1f) * 2.5f, 1.5f + cosf(runTime * 0.6f) * 0.8f, -1.5f + cosf(runTime * 1.1f) * 2.5f };
        lights[3].position = pos3;
        lightEntities[3]->getTransform().setPosition(pos3);
    }

    updateInstancedOrbit(instancedCubes, cubeOrbitData);
    updateInstancedOrbit(instancedSpheres, sphereOrbitData);
}