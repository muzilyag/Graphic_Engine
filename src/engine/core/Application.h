#ifndef APPLICATION_H
#define APPLICATION_H
#include "engine/platform/Window.h"
#include "engine/renderer/Renderer.h"
#include "engine/resources/ResourceManager.h"
#include "engine/scene/Camera.h"
#include "engine/scene/Scene.h"

class Application
{
    bool wasCursorLocked = false;
    void updateInput(float dt);
protected:
    Window window;
    ResourceManager resources;
    Renderer renderer;
    Scene scene;
    Camera camera;
    float runTime = 0.0f;

    virtual void onInit() {}
    virtual void onUpdate(float dt) {}
    virtual void onRender();
    virtual void onShutdown() {}
public:
    Application(int width = 800, int height = 600, const char* title = "Engine");
    virtual ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();
};
#endif