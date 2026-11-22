#include "sandbox/SandboxApp.h"
#include "engine/renderer/Primitives.h"
#include <cmath>
#include <random>

namespace
{
    Mat4 buildFastTRS(float px, float py, float pz, float rotY, float rotX, float scale)
    {
        float cy = cosf(rotY);
        float sy = sinf(rotY);
        float cx = cosf(rotX);
        float sx = sinf(rotX);

        Mat4 r;
        r.m[0]  = cy * scale;
        r.m[1]  = 0.0f;
        r.m[2]  = -sy * scale;
        r.m[3]  = 0.0f;

        r.m[4]  = sy * sx * scale;
        r.m[5]  = cx * scale;
        r.m[6]  = cy * sx * scale;
        r.m[7]  = 0.0f;

        r.m[8]  = sy * cx * scale;
        r.m[9]  = -sx * scale;
        r.m[10] = cy * cx * scale;
        r.m[11] = 0.0f;

        r.m[12] = px;
        r.m[13] = py;
        r.m[14] = pz;
        r.m[15] = 1.0f;
        return r;
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

    auto defaultShader         = resources.loadShader("default", "./assets/shaders/shader.vert", "./assets/shaders/shader.frag");
    auto defaultInstShader     = resources.loadShader("default_instanced", "./assets/shaders/shader_instanced.vert", "./assets/shaders/shader.frag");
    auto unlitShader           = resources.loadShader("unlit", "./assets/shaders/unlit.vert", "./assets/shaders/unlit.frag");
    auto skyboxShader          = resources.loadShader("skybox", "./assets/shaders/skybox.vert", "./assets/shaders/skybox.frag");
    auto shadowShader          = resources.loadShader("shadow", "./assets/shaders/shadow.vert", "./assets/shaders/shadow.frag");
    auto shadowInstancedShader = resources.loadShader("shadow_instanced", "./assets/shaders/shadow_instanced.vert", "./assets/shaders/shadow.frag");
    auto blurShader            = resources.loadShader("blur", "./assets/shaders/fullscreen.vert", "./assets/shaders/blur.frag");
    auto postProcessShader     = resources.loadShader("postprocess", "./assets/shaders/fullscreen.vert", "./assets/shaders/postprocess.frag");
    auto fxaaShader            = resources.loadShader("fxaa", "./assets/shaders/fullscreen.vert", "./assets/shaders/fxaa.frag");

    renderer.setSkybox(cubeMesh, skyboxShader);
    renderer.setShadowShader(shadowShader, shadowInstancedShader);
    renderer.setPostProcessShaders(blurShader, postProcessShader, fxaaShader);
    renderer.setExposure(1.0f);
    renderer.setBloomStrength(0.45f);

    DirectionalLight sun;
    sun.direction = {-0.45f, -0.65f, 0.6f};
    sun.color     = {1.0f, 0.95f, 0.88f};
    sun.intensity = 3.2f;
    sun.enabled   = true;
    scene.setDirectionalLight(sun);

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

    Material glassCyan;
    glassCyan.albedo      = {0.15f, 0.78f, 0.95f};
    glassCyan.metallic    = 0.05f;
    glassCyan.roughness   = 0.06f;
    glassCyan.opacity     = 0.35f;
    glassCyan.transparent = true;
    glassCyan.doubleSided = true;

    Material glassAmber = glassCyan;
    glassAmber.albedo = {0.95f, 0.55f, 0.12f};

    Material glassMagenta = glassCyan;
    glassMagenta.albedo = {0.95f, 0.18f, 0.72f};

    Material glassSphereMat = glassCyan;
    glassSphereMat.opacity     = 0.38f;
    glassSphereMat.doubleSided = false;

    Entity& floor = scene.createEntity("floor", planeMesh, defaultShader, texturedStone);
    floor.getTransform().setPosition({0.0f, -3.5f, -4.0f});

    sphereEntity = &scene.createEntity("sphere", sphereMesh, defaultShader, texturedStone);
    sphereEntity->getTransform().setPosition({0.0f, 1.6f, -2.0f});

    Entity& testMatte = scene.createEntity("test_matte", smallSphere, defaultShader, matteRed);
    testMatte.getTransform().setPosition({-3.2f, -2.5f, -1.5f});

    Entity& testGlossy = scene.createEntity("test_glossy", smallSphere, defaultShader, glossyBlue);
    testGlossy.getTransform().setPosition({-1.9f, -2.5f, -1.5f});

    if (pyramidMesh)
    {
        pyramidEntity = &scene.createEntity("pyramid", pyramidMesh, defaultShader, pyramidMat);
        pyramidEntity->getTransform().setPosition({-0.6f, -2.95f, -1.5f});
    }

    Entity& testGold = scene.createEntity("test_gold", smallSphere, defaultShader, polishedGold);
    testGold.getTransform().setPosition({0.7f, -2.5f, -1.5f});

    Entity& testChrome = scene.createEntity("test_chrome", smallSphere, defaultShader, brushedChrome);
    testChrome.getTransform().setPosition({2.0f, -2.5f, -1.5f});

    Entity& testMissing = scene.createEntity("test_missing_tex", smallSphere, defaultShader, missingTexMat);
    testMissing.getTransform().setPosition({3.3f, -2.5f, -1.5f});

    Quat upright = Quat::fromAxisAngle({1.0f, 0.0f, 0.0f}, 1.5707963f);

    Entity& glass1 = scene.createEntity("glass_cyan", glassMesh, defaultShader, glassCyan);
    glass1.getTransform().setPosition({-0.6f, -2.2f, 0.2f});
    glass1.getTransform().setRotation(upright);
    glass1.setCastsShadows(false);

    Entity& glass2 = scene.createEntity("glass_amber", glassMesh, defaultShader, glassAmber);
    glass2.getTransform().setPosition({0.0f, -2.2f, 0.8f});
    glass2.getTransform().setRotation(upright);
    glass2.setCastsShadows(false);

    Entity& glass3 = scene.createEntity("glass_magenta", glassMesh, defaultShader, glassMagenta);
    glass3.getTransform().setPosition({0.6f, -2.2f, 1.4f});
    glass3.getTransform().setRotation(upright);
    glass3.setCastsShadows(false);

    Entity& glassSphere = scene.createEntity("glass_sphere", sphereMesh, defaultShader, glassSphereMat);
    glassSphere.getTransform().setPosition({-2.5f, 0.2f, -2.5f});
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
    std::uniform_real_distribution<float> angleDist(0.0f, 6.2831853f);
    std::uniform_real_distribution<float> cubeRadiusDist(10.5f, 65.0f);
    std::uniform_real_distribution<float> sphereRadiusDist(12.0f, 60.0f);
    std::uniform_real_distribution<float> heightDist(-3.0f, 24.0f);
    std::uniform_real_distribution<float> speedDist(0.04f, 0.22f);
    std::uniform_real_distribution<float> rotDist(-1.8f, 1.8f);
    std::uniform_real_distribution<float> scaleDist(0.25f, 0.95f);
    std::uniform_real_distribution<float> bobAmpDist(0.2f, 1.2f);
    std::uniform_real_distribution<float> bobSpeedDist(0.4f, 1.4f);

    constexpr int CUBE_INSTANCES = 3500;
    constexpr int SPHERE_INSTANCES = 1500;

    cubeOrbitData.reserve(CUBE_INSTANCES);
    instancedCubes->transforms.resize(CUBE_INSTANCES);
    for (int i = 0; i < CUBE_INSTANCES; ++i)
    {
        OrbitInstanceData d;
        d.radius     = cubeRadiusDist(rng);
        d.baseAngle  = angleDist(rng);
        d.orbitSpeed = speedDist(rng) * ((i % 2 == 0) ? 1.0f : -1.0f);
        d.height     = heightDist(rng);
        d.bobAmp     = bobAmpDist(rng);
        d.bobSpeed   = bobSpeedDist(rng);
        d.rotSpeedY  = rotDist(rng);
        d.rotSpeedX  = rotDist(rng);
        d.scale      = scaleDist(rng);
        cubeOrbitData.push_back(d);
    }

    sphereOrbitData.reserve(SPHERE_INSTANCES);
    instancedSpheres->transforms.resize(SPHERE_INSTANCES);
    for (int i = 0; i < SPHERE_INSTANCES; ++i)
    {
        OrbitInstanceData d;
        d.radius     = sphereRadiusDist(rng);
        d.baseAngle  = angleDist(rng);
        d.orbitSpeed = speedDist(rng) * ((i % 2 == 0) ? -1.0f : 1.0f);
        d.height     = heightDist(rng) + 1.5f;
        d.bobAmp     = bobAmpDist(rng);
        d.bobSpeed   = bobSpeedDist(rng);
        d.rotSpeedY  = rotDist(rng);
        d.rotSpeedX  = rotDist(rng);
        d.scale      = scaleDist(rng) * 1.1f;
        sphereOrbitData.push_back(d);
    }

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
        float px = CENTER_X + cosf(a) * d.radius;
        float pz = CENTER_Z + sinf(a) * d.radius;
        float py = d.height + sinf(runTime * d.bobSpeed + d.baseAngle) * d.bobAmp;
        batch->transforms[i] = buildFastTRS(px, py, pz, runTime * d.rotSpeedY, runTime * d.rotSpeedX, d.scale);
    }
}

void SandboxApp::onUpdate(float dt)
{
    (void)dt;

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