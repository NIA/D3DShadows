#pragma once
#include "main.h"
#include "Camera.h"
#include "Window.h"
#include "Vertex.h"
#include "Model.h"

#pragma warning( disable : 4996 ) // disable deprecated warning 
#pragma warning( disable : 4995 ) // disable deprecated warning 
#include <list>
#pragma warning( default : 4996 ) // disable deprecated warning
#pragma warning( default : 4995 ) // disable deprecated warning 

extern const unsigned VECTORS_IN_MATRIX;
typedef std::list<Model*> Models;

class Application
{
private:
    IDirect3D9                  *d3d;           // used to create the D3DDevice
    IDirect3DDevice9            *device;        // our rendering device

    bool point_light_enabled;
    bool ambient_light_enabled;

    Window window;

    Models models;
    Plane *plane;
    LightSource *light_source;

    Camera camera;

    D3DXVECTOR3 point_light_position;

    // Initialization steps:
    void init_device();

    // Wrappers for SetVertexShaderConstantF:
    void set_shader_const(unsigned reg, const float *data, unsigned vector4_count)
    {
        check_render( device->SetVertexShaderConstantF(reg, data, vector4_count) );
    }
    void set_shader_float(unsigned reg, float f)
    {
        set_shader_const(reg, D3DXVECTOR4(f, f, f, f), 1);
    }
    void set_shader_vector(unsigned reg, const D3DXVECTOR3 &vector)
    {
        set_shader_const(reg, D3DXVECTOR4(vector, 0), 1);
    }
    void set_shader_point(unsigned reg, const D3DXVECTOR3 &point)
    {
        set_shader_const(reg, D3DXVECTOR4(point, 1.0f), 1);
    }
    void set_shader_matrix(unsigned reg, const D3DXMATRIX &matrix)
    {
        set_shader_const(reg, matrix, VECTORS_IN_MATRIX);
    }
    void set_shader_color(unsigned reg, D3DCOLOR color)
    {
        set_shader_const(reg, D3DXCOLOR(color), 1);
    }
    // Wrapper for SetRenderState:
    void set_render_state( D3DRENDERSTATETYPE state, DWORD value )
    {
        check_state( device->SetRenderState(state, value) );
    }

    void rotate_models(float phi);
    void process_key(unsigned code);

    void draw_model(Model *model, float time, bool shadow);
    void render();

    // Deinitialization steps:
    void release_interfaces();

public:
    Application();
    IDirect3DDevice9 * get_device();

    void add_model(Model &model);
    void set_plane(Plane &_plane) { plane = &_plane; }
    void set_light_source_model(LightSource &_light_source) { light_source = &_light_source; }
    void remove_model(Model &model);
    void run();

    void toggle_wireframe();

    ~Application();

};
