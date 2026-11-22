#include "engine/platform/Window.h"
#include "engine/platform/GLLoader.h"

Window::Window(int w, int h, const char* title) : logger("Window")
{
    WNDCLASSA wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "GL_WINDOW_CLASS";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&wc);

    hwnd = CreateWindowExA(
        0, wc.lpszClassName, title,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        100, 100, w, h,
        NULL, NULL, wc.hInstance, this
    );
    hdc = GetDC(hwnd);

    PIXELFORMATDESCRIPTOR pfd = { sizeof(pfd), 1 };
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;

    SetPixelFormat(hdc, ChoosePixelFormat(hdc, &pfd), &pfd);

    HGLRC tempContext = wglCreateContext(hdc);
    wglMakeCurrent(hdc, tempContext);

    auto wglCreateContextAttribsARB =
        (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

    if (wglCreateContextAttribsARB)
    {
        int attribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            0
        };

        hrc = wglCreateContextAttribsARB(hdc, nullptr, attribs);
        if (hrc)
        {
            wglMakeCurrent(hdc, hrc);
            wglDeleteContext(tempContext);
        }
        else
        {
            logger.warn("Failed to create OpenGL 3.3 Core context, falling back to legacy.");
            hrc = tempContext;
        }
    }
    else
    {
        logger.warn("wglCreateContextAttribsARB not supported, using legacy context.");
        hrc = tempContext;
    }

    loadExtensions();

    const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    logger.info("OpenGL Version: " + std::string(version ? version : "Unknown"));
    logger.info("GPU Renderer: " + std::string(renderer ? renderer : "Unknown"));

    RECT r;
    GetClientRect(hwnd, &r);
    width = r.right;
    height = r.bottom;
    if (width > 0 && height > 0)
    {
        glViewport(0, 0, width, height);
    }

    setCursorLocked(true);
}

Window::~Window() 
{
    setCursorLocked(false);

    if (hrc)
    {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(hrc);
        hrc = nullptr;
    }
    if (hdc && hwnd)
    {
        ReleaseDC(hwnd, hdc);
        hdc = nullptr;
    }
}

bool Window::shouldClose() 
{
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
    {
        if (msg.message == WM_QUIT) 
        {
            return true;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return false;
}

void Window::swapBuffers() 
{
    SwapBuffers(hdc);
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) 
{
    Window* win = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    switch (msg) 
    {
        case WM_CREATE: 
        {
            LPCREATESTRUCT pcs = (LPCREATESTRUCT)lp;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pcs->lpCreateParams);
            return 0;
        }
        case WM_LBUTTONDOWN:
        {
            if (win)
            {
                win->setCursorLocked(true);
            }
            return 0;
        }
        case WM_KILLFOCUS:
        {
            if (win)
            {
                win->setCursorLocked(false);
                for (int i = 0; i < 256; ++i)
                {
                    win->keys[i] = false;
                }
            }
            return 0;
        }
        case WM_KEYDOWN: 
        {
            if (win) 
            {
                if (wp == VK_ESCAPE)
                {
                    win->setCursorLocked(false);
                }
                if (wp < 256)
                {
                    win->keys[wp] = true;
                }
            }
            return 0;
        }
        case WM_KEYUP:   
        {
            if (win && wp < 256)
            {
                win->keys[wp] = false;
            }
            return 0;
        }
        case WM_SIZE:
        {
            if (win)
            {
                win->width = LOWORD(lp);
                win->height = HIWORD(lp);
                if (win->hrc && win->width > 0 && win->height > 0)
                {
                    glViewport(0, 0, win->width, win->height);
                }
            }
            return 0;
        }
        case WM_DESTROY: 
        {
            PostQuitMessage(0); 
            return 0;
        }
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}

void Window::setCursorLocked(bool locked)
{
    if (isCursorLocked == locked)
    {
        return;
    }
    isCursorLocked = locked;
    ShowCursor(!locked);
}