#pragma once
#include <windows.h>
#include "main.h"

class Window
{
private:
    HWND hwnd;
    WNDCLASSEX window_class;

    void unregister_class();
public:
    Window(int width, int height);
    void show() const;
    void update() const;
    static LRESULT WINAPI MsgProc( HWND, UINT, WPARAM, LPARAM );

    inline operator HWND() { return this->hwnd; }
    inline operator HWND() const { return this->hwnd; }
    ~Window();
};