#include "Shader.h"

VertexShader::VertexShader(IDirect3DDevice9 *device, const D3DVERTEXELEMENT9* vertex_declaration, const char *shader_filename)
: device(device), vertex_decl(NULL), shader(NULL)
{
    try
    {
        if( FAILED( device->CreateVertexDeclaration(vertex_declaration, &vertex_decl) ) )
            throw VertexDeclarationInitError();

        ID3DXBuffer * shader_buffer = NULL;
        try
        {
            if( FAILED( D3DXAssembleShaderFromFileA( shader_filename, NULL, NULL, NULL, &shader_buffer, NULL ) ) )
                throw VertexShaderAssemblyError();
            if( FAILED( device->CreateVertexShader( (DWORD*) shader_buffer->GetBufferPointer(), &shader ) ) )
                throw VertexShaderInitError();
        }
        // using catch(...) because every caught exception is rethrown
        catch(...)
        {
            release_interface(shader_buffer);
            throw;
        }
        release_interface(shader_buffer);
    }
    // using catch(...) because every caught exception is rethrown
    catch(...)
    {
        release_interfaces();
        throw;
    }
}

void VertexShader::set()
{
    check_render( device->SetVertexDeclaration(vertex_decl) );
    check_render( device->SetVertexShader(shader) );
}

void VertexShader::release_interfaces()
{
    release_interface( vertex_decl );
    release_interface( shader );
}

VertexShader::~VertexShader()
{
    release_interfaces();
}
