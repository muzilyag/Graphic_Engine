#include "engine/core/Application.h"
#include "engine/platform/GLLoader.h"
#include <iomanip>
#include <sstream>

Application::Application(int width, int height, const char* title)
    : baseTitle(title ? title : "Engine"), window(width, height, title)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    resources.initDefaults();
    renderer.init(resources);
    runTime = 0.0f;
}

void Application::run()
{
    onInit();

    long long last, freq;
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    QueryPerformanceCounter((LARGE_INTEGER*)&last);

    while (!window.shouldClose())
    {
        long long now;
        QueryPerformanceCounter((LARGE_INTEGER*)&now);
        float rawDt = (float)(now - last) / freq;
        last = now;

        float dt = rawDt;
        if (dt > 0.1f)
        {
            dt = 0.1f;
        }

        runTime += dt;
        updateInput(dt);
        onUpdate(dt);
        onRender();
        window.swapBuffers();
        updateWindowTitle(rawDt);
    }

    onShutdown();
    scene.clear();
    renderer.shutdown();
    resources.clear();
}

void Application::updateWindowTitle(float dt)
{
    fpsAccumulator += dt;
    ++frameCounter;

    if (fpsAccumulator >= 0.25f)
    {
        float fps = static_cast<float>(frameCounter) / fpsAccumulator;
        float ms = (fpsAccumulator / static_cast<float>(frameCounter)) * 1000.0f;
        const RenderStats& st = renderer.getStats();

        std::ostringstream oss;
        oss << baseTitle << " | " << static_cast<int>(fps + 0.5f)
            << " FPS (" << std::fixed << std::setprecision(2) << ms << " ms)"
            << " | Vis: " << st.visibleObjects << "/" << st.totalObjects
            << " | DC: " << st.drawCalls;
        if (wireframeMode)
        {
            oss << " [Wireframe]";
        }
        if (!renderer.isBloomEnabled())
        {
            oss << " [No Bloom]";
        }
        if (!renderer.isFxaaEnabled())
        {
            oss << " [No FXAA]";
        }
        if (!renderer.isFrustumCullingEnabled())
        {
            oss << " [No Cull]";
        }
        if (!renderer.isSSAOEnabled())
        {
            oss << " [No SSAO]";
        }
        if (!renderer.isRefractionEnabled())
        {
            oss << " [No Refract]";
        }
        window.setTitle(oss.str());

        fpsAccumulator = 0.0f;
        frameCounter = 0;
    }
}

void Application::updateInput(float dt)
{
    bool canControlMouse = window.getCursorLocked()
                        && window.isFocused()
                        && window.getWidth() > 0
                        && window.getHeight() > 0;

    if (canControlMouse)
    {
        int midX = window.getWidth() / 2;
        int midY = window.getHeight() / 2;

        POINT ctr = { midX, midY };
        ClientToScreen(window.getHwnd(), &ctr);

        if (!wasCursorLocked)
        {
            SetCursorPos(ctr.x, ctr.y);
            camera.setLastX((float)midX);
            camera.setLastY((float)midY);
            wasCursorLocked = true;
        }
        else
        {
            POINT mp;
            GetCursorPos(&mp);
            ScreenToClient(window.getHwnd(), &mp);
            camera.processMouse((float)mp.x, (float)mp.y);

            SetCursorPos(ctr.x, ctr.y);
            camera.setLastX((float)midX);
            camera.setLastY((float)midY);
        }
    }
    else
    {
        wasCursorLocked = false;
    }

    if (window.isFocused())
    {
        bool* keys = window.getKeys();
        camera.processKeyboard(keys, dt);

        bool f5Down = keys[VK_F5];
        if (f5Down && !wasF5Down)
        {
            resources.reloadAllShaders();
        }
        wasF5Down = f5Down;

        bool f2Down = keys[VK_F2];
        if (f2Down && !wasF2Down)
        {
            wireframeMode = !wireframeMode;
            renderer.setWireframe(wireframeMode);
        }
        wasF2Down = f2Down;

        bool f3Down = keys[VK_F3];
        if (f3Down && !wasF3Down)
        {
            renderer.setBloomEnabled(!renderer.isBloomEnabled());
        }
        wasF3Down = f3Down;

        bool f4Down = keys[VK_F4];
        if (f4Down && !wasF4Down)
        {
            renderer.setFxaaEnabled(!renderer.isFxaaEnabled());
        }
        wasF4Down = f4Down;

        bool f6Down = keys[VK_F6];
        if (f6Down && !wasF6Down)
        {
            renderer.setFrustumCullingEnabled(!renderer.isFrustumCullingEnabled());
        }
        wasF6Down = f6Down;

        bool f7Down = keys[VK_F7];
        if (f7Down && !wasF7Down)
        {
            renderer.setSSAOEnabled(!renderer.isSSAOEnabled());
        }
        wasF7Down = f7Down;

        bool f8Down = keys[VK_F8];
        if (f8Down && !wasF8Down)
        {
            renderer.setRefractionEnabled(!renderer.isRefractionEnabled());
        }
        wasF8Down = f8Down;
    }
    else
    {
        wasF5Down = false;
        wasF2Down = false;
        wasF3Down = false;
        wasF4Down = false;
        wasF6Down = false;
        wasF7Down = false;
        wasF8Down = false;
    }
}

void Application::onRender()
{
    int w = window.getWidth();
    int h = window.getHeight();
    if (w == 0 || h == 0)
    {
        return;
    }

    float aspect = static_cast<float>(w) / static_cast<float>(h);
    Mat4 view = camera.getViewMatrix();
    Mat4 projection = Mat4::perspective(0.785f, aspect, 0.1f, 150.0f);

    renderer.setClearColor(0.02f, 0.02f, 0.04f, 1.0f);
    renderer.beginFrame(view, projection, camera.getPos(), w, h);
    scene.submit(renderer);
    renderer.flush();
}