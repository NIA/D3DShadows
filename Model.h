#pragma once
#include "main.h"
#include "Vertex.h"
#include "Shader.h"

class Model
{
private:
    IDirect3DDevice9    *device;
    VertexShader        &shader;

    unsigned    vertices_count;
    unsigned    primitives_count;

    D3DPRIMITIVETYPE        primitive_type;
    IDirect3DVertexBuffer9  *vertex_buffer;
    IDirect3DIndexBuffer9   *index_buffer;
    unsigned vertex_size;

    D3DXVECTOR3 position;
    D3DXVECTOR3 rotation;
    D3DXMATRIX rotation_and_position;

    void update_matrix();

    void release_interfaces();

public:
    Model(  IDirect3DDevice9 *device,
            D3DPRIMITIVETYPE primitive_type,
            VertexShader &shader,
            unsigned vertex_size,
            const Vertex *vertices,
            unsigned vertices_count,
            const Index *indices,
            unsigned indices_count,
            unsigned primitives_count,
            D3DXVECTOR3 position,
            D3DXVECTOR3 rotation);
    
    VertexShader &get_shader();
    virtual void set_time(float time) = 0;
    virtual unsigned set_constants(D3DXVECTOR4 *out_data, unsigned buffer_size) const = 0; // returns number of constants used
    
    const D3DXMATRIX &get_rotation_and_position() const;
    void rotate(float phi);
    
    virtual void draw() const;

    virtual ~Model();
private:
    // No copying!
    Model(const Model&);
    Model &operator=(const Model&);
};

class SkinningModel : public Model
{
private:
    D3DXVECTOR3 bone_center;
    D3DXMATRIX bones[BONES_COUNT];
public:
    SkinningModel(  IDirect3DDevice9 *device,
                    D3DPRIMITIVETYPE primitive_type,
                    VertexShader &shader,
                    const SkinningVertex *vertices,
                    unsigned vertices_count,
                    const Index *indices,
                    unsigned indices_count,
                    unsigned primitives_count,
                    D3DXVECTOR3 position,
                    D3DXVECTOR3 rotation,
                    D3DXVECTOR3 bone_center);

    // Overrides:
    virtual void set_time(float time);
    virtual unsigned set_constants(D3DXVECTOR4 *out_data, unsigned buffer_size) const; // returns number of constants used
};

class MorphingModel : public Model
{
private:
    float morphing_param;
    float final_radius;
public:
    MorphingModel(  IDirect3DDevice9 *device,
                    D3DPRIMITIVETYPE primitive_type,
                    VertexShader &shader,
                    const Vertex *vertices,
                    unsigned vertices_count,
                    const Index *indices,
                    unsigned indices_count,
                    unsigned primitives_count,
                    D3DXVECTOR3 position,
                    D3DXVECTOR3 rotation,
                    float final_radius);

    // Overrides:
    virtual void set_time(float time);
    virtual unsigned set_constants(D3DXVECTOR4 *out_data, unsigned buffer_size) const; // returns number of constants used
};
