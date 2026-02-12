#ifndef WINDOW_H
#define WINDOW_H
#include <Windows.h>
#include "Logger.h"

class Window 
{
    Logger logger;

    HWND hwnd;
    HDC hdc;
    HGLRC hrc;
    bool keys[256] = { false };
    bool isCursorLocked = true;

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
public:
    Window(int w, int h, const char* title);
    ~Window();
    bool shouldClose();
    void swapBuffers();
    // Getters
    bool* getKeys() { return keys;}
    HWND getHwnd() {return hwnd; }
    bool getCursorLocked() {return isCursorLocked; }
    int getWidth() const { RECT r; GetClientRect(hwnd, &r); return r.right; }
    int getHeight() const { RECT r; GetClientRect(hwnd, &r); return r.bottom; }
    // Setters
    void setCursorLocked(const bool& locked);
};
#endif