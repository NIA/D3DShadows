#include "Window.h"

namespace
{
    const TCHAR *WINDOW_CLASS = _T("Shadows");
    const TCHAR *WINDOW_TITLE = _T("Shadows");
}

Window::Window(int width, int height)
{
    ZeroMemory( &window_class, sizeof(window_class) );

    window_class.cbSize = sizeof( WNDCLASSEX );
    window_class.style = CS_CLASSDC;
    window_class.lpfnWndProc = Window::MsgProc;
    window_class.hInstance = GetModuleHandle( NULL );
    window_class.lpszClassName = WINDOW_CLASS;

    RegisterClassEx( &window_class );

    hwnd = CreateWindow( WINDOW_CLASS, WINDOW_TITLE,
                         WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height,
                         NULL, NULL, window_class.hInstance, NULL );
    if( NULL == hwnd )
    {
        unregister_class();
        throw WindowInitError();
    }
}

LRESULT WINAPI Window::MsgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    switch( msg )
    {
        case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hwnd, msg, wparam, lparam );
}

void Window::show() const
{
    ShowWindow( *this, SW_SHOWDEFAULT );
}

void Window::update() const
{
    UpdateWindow( *this );
}

void Window::unregister_class()
{
    UnregisterClass( WINDOW_CLASS, window_class.hInstance );
}

Window::~Window()
{
    unregister_class();
}

