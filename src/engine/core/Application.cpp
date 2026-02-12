#include "Application.h"

Application::Application() : window(800, 600, "Engine") 
{
    glEnable(GL_DEPTH_TEST);

    float vertices[] = {
        // Позиции           // Нормали           // Текстурные координаты
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    cubeMesh = std::make_unique<Mesh>(vertices, sizeof(vertices));
    cubeShader = std::make_unique<Shader>("./assets/shaders/shader.vert", "./assets/shaders/shader.frag");
    cubeTexture = std::make_unique<Texture>("./assets/textures/i1.bmp");
    specularMap = std::make_unique<Texture>("./assets/textures/spec.bmp");
    normalMap = std::make_unique<Texture>("./assets/textures/kamen3.bmp");

    runTime = 0.f;
}

void Application::run() 
{
    long long last, freq;
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    QueryPerformanceCounter((LARGE_INTEGER*)&last);

    while (!window.shouldClose()) 
    {
        long long now;
        QueryPerformanceCounter((LARGE_INTEGER*)&now);
        float dt = (float)(now - last) / freq;
        last = now;

        if(dt > 0.1f)
        {
            dt = 0.1f;
        }

        update(dt);
        render();
        window.swapBuffers();
    }
}

void Application::update(float dt) 
{
    runTime += dt;

    if(window.getCursorLocked())
    {
        int midX = window.getWidth() / 2;
        int midY = window.getHeight() / 2;

        POINT mp;
        GetCursorPos(&mp);
        ScreenToClient(window.getHwnd(), &mp);
        camera.processMouse((float)mp.x, (float)mp.y);
        
        POINT ctr = { midX, midY };
        ClientToScreen(window.getHwnd(), &ctr);
        SetCursorPos(ctr.x, ctr.y);

        camera.setLastX((float)midX);
        camera.setLastY((float)midY);
    }
    camera.processKeyboard(window.getKeys(), dt);
}

void Application::render() 
{
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float width = (float)window.getWidth();
    float height = (float)window.getHeight();
    if (height == 0) 
    {
        height = 1.0f;
    }
    float aspect = width / height;

    cubeShader->use();
    cubeTexture->bind(0);
    specularMap->bind(1);
    normalMap->bind(2);
    cubeShader->setInt("material.diffuseMap", 0);
    cubeShader->setInt("material.specularMap", 1);
    cubeShader->setInt("material.normalMap", 2);

    Vec3 currentLightPos = { sinf(runTime) * 4.0f, 2.0f, cosf(runTime) * 4.0f };

    // cubeShader->setVec3("lightPos", currentLightPos);
    // cubeShader->setVec3("viewPos", camera.getPos());
    // cubeShader->setVec3("lightColor", {sinf(runTime), cosf(runTime), 1.0f});

    for (int i = 0; i < 4; i++) {
        std::string base = "pointLights[" + std::to_string(i) + "].";
        cubeShader->setVec3((base + "position").c_str(), lightPositions[i]);
        cubeShader->setVec3((base + "color").c_str(), lightColors[i]);
        
        // Стандартные коэффициенты для того, чтобы свет бил примерно на 50 метров
        cubeShader->setFloat((base + "constant").c_str(), 1.0f);
        cubeShader->setFloat((base + "linear").c_str(), 0.09f);
        cubeShader->setFloat((base + "quadratic").c_str(), 0.032f);
    }

    Mat4 view = camera.getViewMatrix();
    Mat4 projection = Mat4::perspective(0.785f, aspect, 0.1f, 100.0f);
    cubeShader->setMat4("view", view);
    cubeShader->setMat4("projection", projection);

    Material gold = {
        {0.247f, 0.199f, 0.074f},
        {0.751f, 0.606f, 0.226f},
        {0.628f, 0.555f, 0.366f},
        51.2f 
    };

    cubeShader->setMaterial("material", gold);

    Transform transform;
    for (int i = 0; i < 5; i++)
    {
        transform.setPosiotn(positions[i]);
        float angle = runTime * (i + 1);
        Quat q = Quat::fromAxisAngle({1.0f, 0.3f, 0.5f}, angle); 
        transform.setRotation(q);
        cubeShader->setMat4("model", transform.getModelMatrix());
        cubeMesh->draw();
    }
}