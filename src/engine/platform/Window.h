#ifndef WINDOW_H
#define WINDOW_H
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <string>
#include "engine/utils/Logger.h"

class Window 
{
    Logger logger;

    HWND hwnd = nullptr;
    HDC hdc = nullptr;
    HGLRC hrc = nullptr;
    int width = 0;
    int height = 0;
    bool keys[256] = { false };
    bool isCursorLocked = false;

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
public:
    Window(int w, int h, const char* title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose();
    void swapBuffers();
    // Getters
    bool* getKeys() { return keys; }
    const bool* getKeys() const { return keys; }
    HWND getHwnd() const { return hwnd; }
    bool getCursorLocked() const { return isCursorLocked; }
    bool isFocused() const { return hwnd != nullptr && GetForegroundWindow() == hwnd; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    // Setters
    void setCursorLocked(bool locked);
    void setTitle(const std::string& title);
};
#endif