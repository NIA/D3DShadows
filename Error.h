#pragma once
#include <exception>
#include <tchar.h>

class RuntimeError : public std::exception
{
private:
    const TCHAR * msg;
public:
    RuntimeError(const TCHAR *msg) :  msg(msg) {}
    const TCHAR *message() const { return msg; }
};

class WindowInitError : public RuntimeError
{
public:
    WindowInitError() : RuntimeError( _T("Error while creating window") ) {}
};
class D3DInitError : public RuntimeError
{
public:
    D3DInitError() : RuntimeError( _T("Error while initializing D3D device") ) {}
};
class VertexDeclarationInitError : public RuntimeError
{
public:
    VertexDeclarationInitError() : RuntimeError( _T("Error while creating vertex declaration") ) {}
};
class VertexShaderAssemblyError : public RuntimeError
{
public:
    VertexShaderAssemblyError() : RuntimeError( _T("Error while assembling vertex shader") ) {}
};
class VertexShaderInitError : public RuntimeError
{
public:
    VertexShaderInitError() : RuntimeError( _T("Error while creating vertex shader") ) {}
};
class VertexBufferInitError : public RuntimeError
{
public:
    VertexBufferInitError() : RuntimeError( _T("Error while creating vertex buffer") ) {}
};
class IndexBufferInitError : public RuntimeError
{
public:
    IndexBufferInitError() : RuntimeError( _T("Error while creating index buffer") ) {}
};
class VertexBufferFillError : public RuntimeError
{
public:
    VertexBufferFillError() : RuntimeError( _T("Error while filling vertex buffer") ) {}
};
class IndexBufferFillError : public RuntimeError
{
public:
    IndexBufferFillError() : RuntimeError( _T("Error while filling index buffer") ) {}
};
class RenderError : public RuntimeError
{
public:
    RenderError() : RuntimeError( _T("Error while rendering scene") ) {}
};
class RenderStateError : public RuntimeError
{
public:
    RenderStateError() : RuntimeError( _T("Error while setting render state") ) {}
};

inline void check_render( HRESULT res )
{
    if( FAILED(res) )
        throw RenderError();
}

inline void check_state( HRESULT res )
{
    if( FAILED(res) )
        throw RenderStateError();
}
