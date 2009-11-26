#pragma once

#include "main.h"

class VertexShader
{
private:
    IDirect3DDevice9            *device;

    IDirect3DVertexDeclaration9 *vertex_decl;   // vertex declaration
    IDirect3DVertexShader9      *shader;        // vertex shader
    
    // Deinitialization steps:
    void release_interfaces();
public:
    VertexShader(IDirect3DDevice9 *device, const D3DVERTEXELEMENT9* vertex_declaration, const char *shader_filename);
    void set();
    ~VertexShader();
};