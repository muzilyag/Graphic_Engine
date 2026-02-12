#include "Window.h"
#include "GLLoader.h"

Window::Window(int w, int h, const char* title) : logger("Window")
{
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "GL_WINDOW_CLASS";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&wc);

    hwnd = CreateWindowExA(0, wc.lpszClassName, title, WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, w, h, NULL, NULL, wc.hInstance, this);
    hdc = GetDC(hwnd);

    PIXELFORMATDESCRIPTOR pfd = { sizeof(pfd), 1 };
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;

    SetPixelFormat(hdc, ChoosePixelFormat(hdc, &pfd), &pfd);
    hrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hrc);
    loadExtensions();

    ShowCursor(FALSE);
}

Window::~Window() 
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hrc);
    ReleaseDC(hwnd, hdc);
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
    switch(msg) 
    {
        case WM_CREATE: 
        {
            LPCREATESTRUCT pcs = (LPCREATESTRUCT)lp;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pcs->lpCreateParams);
            return 0;
        }
        case WM_LBUTTONDOWN:
        {
            if(win)
            {
                win->setCursorLocked(true);
            }
            break;
        }
        case WM_KEYDOWN: 
        {
            if(win) 
            {
                if(wp == VK_ESCAPE)
                {
                    win->setCursorLocked(false);   
                }
                win->keys[wp] = true; 
                return 0;
            }
        }
        case WM_SIZE:
        {
            glViewport(0, 0, LOWORD(lp), HIWORD(lp));
            return 0;
        }
        case WM_KEYUP:   
        {
            if(win)
            {
                win->keys[wp] = false; 
                return 0;
            }
        }
        case WM_DESTROY: 
        {
            PostQuitMessage(0); 
            return 0;
        }
        case WM_MOUSEMOVE: 
        {
            if(win) 
            {
                int x = LOWORD(lp);
                int y = HIWORD(lp);
            }
            return 0;
        }
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}

void Window::setCursorLocked(const bool& locked)
{
    isCursorLocked = locked;
    ShowCursor(!locked);
}