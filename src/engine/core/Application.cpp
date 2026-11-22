#include "engine/core/Application.h"
#include "engine/platform/GLLoader.h"

Application::Application(int width, int height, const char* title) : window(width, height, title)
{
    glEnable(GL_DEPTH_TEST);
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
        float dt = (float)(now - last) / freq;
        last = now;

        if (dt > 0.1f)
        {
            dt = 0.1f;
        }

        runTime += dt;
        updateInput(dt);
        onUpdate(dt);
        onRender();
        window.swapBuffers();
    }

    onShutdown();
    scene.clear();
    renderer.shutdown();
    resources.clear();
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
        camera.processKeyboard(window.getKeys(), dt);
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
    Mat4 projection = Mat4::perspective(0.785f, aspect, 0.1f, 100.0f);

    renderer.clear(0.1f, 0.1f, 0.2f, 1.0f);
    renderer.beginFrame(view, projection, camera.getPos());
    scene.submit(renderer);
    renderer.flush();
}