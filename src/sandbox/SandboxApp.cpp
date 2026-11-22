#include "sandbox/SandboxApp.h"
#include "engine/renderer/Primitives.h"

SandboxApp::SandboxApp() : Application(800, 600, "Engine")
{
}

void SandboxApp::onInit()
{
    auto cubeMesh    = resources.registerMesh("cube", Primitives::createCube());
    auto planeMesh   = resources.registerMesh("plane", Primitives::createPlane(20.0f, 8.0f));
    auto sphereMesh  = resources.registerMesh("sphere", Primitives::createSphere(0.7f, 32, 16));
    auto pyramidMesh = resources.loadMesh("pyramid", "./assets/models/pyramid.obj");

    auto defaultShader = resources.loadShader("default", "./assets/shaders/shader.vert", "./assets/shaders/shader.frag");

    auto cubeTex   = resources.loadTexture("cube_diffuse", "./assets/textures/i1.bmp");
    auto modelTex  = resources.loadTexture("model_diffuse", "./assets/textures/tex.tga");
    auto specMap   = resources.loadTexture("specular", "./assets/textures/spec.bmp");
    auto normalMap = resources.loadTexture("normal", "./assets/textures/kamen3.bmp");

    Material goldMaterial;
    goldMaterial.ambient     = {0.247f, 0.199f, 0.074f};
    goldMaterial.diffuse     = {0.751f, 0.606f, 0.226f};
    goldMaterial.specular    = {0.628f, 0.555f, 0.366f};
    goldMaterial.shininess   = 51.2f;
    goldMaterial.diffuseMap  = cubeTex;
    goldMaterial.specularMap = specMap;
    goldMaterial.normalMap   = normalMap;

    Material pyramidMaterial = goldMaterial;
    pyramidMaterial.diffuseMap = modelTex;

    Entity& floor = scene.createEntity("floor", planeMesh, defaultShader, goldMaterial);
    floor.getTransform().setPosition({0.0f, -3.5f, -4.0f});

    sphereEntity = &scene.createEntity("sphere", sphereMesh, defaultShader, goldMaterial);
    sphereEntity->getTransform().setPosition({0.0f, 1.6f, -2.0f});

    if (pyramidMesh)
    {
        pyramidEntity = &scene.createEntity("pyramid", pyramidMesh, defaultShader, pyramidMaterial);
        pyramidEntity->getTransform().setPosition({0.0f, -2.0f, -1.5f});
    }

    Vec3 cubePositions[5] = {
        { 0.0f,  0.0f,  0.0f},
        { 2.0f,  5.0f, -15.0f},
        {-1.5f, -2.2f, -2.5f},
        {-3.8f, -2.0f, -12.3f},
        { 2.4f, -0.4f, -3.5f}
    };

    for (int i = 0; i < 5; ++i)
    {
        Entity& cube = scene.createEntity("cube_" + std::to_string(i), cubeMesh, defaultShader, goldMaterial);
        cube.getTransform().setPosition(cubePositions[i]);
        cubeEntities.push_back(&cube);
    }

    scene.addPointLight({{ 0.7f,  0.2f,   2.0f}, {1.0f, 0.0f, 0.0f}, 1.0f, 0.09f, 0.032f});
    scene.addPointLight({{ 2.3f, -3.3f,  -4.0f}, {0.0f, 1.0f, 0.0f}, 1.0f, 0.09f, 0.032f});
    scene.addPointLight({{-4.0f,  2.0f, -12.0f}, {0.0f, 0.0f, 1.0f}, 1.0f, 0.09f, 0.032f});
    scene.addPointLight({{ 0.0f,  0.0f,  -3.0f}, {1.0f, 1.0f, 1.0f}, 1.0f, 0.09f, 0.032f});
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
}