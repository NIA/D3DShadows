#include "Model.h"
#include "matrices.h"

namespace
{
    const float SKINNING_PERIOD = 2.0f;
    const float SKINNING_OMEGA = 2.0f*D3DX_PI/SKINNING_PERIOD;
    const float SKINNING_ANGLE = D3DX_PI/8.0f;

    const float MORPHING_PERIOD = 3.0f;
    const float MORPHING_OMEGA = 2.0f*D3DX_PI/MORPHING_PERIOD;

    const unsigned MORPHING_CONSTANTS_USED = 2; // final radius and t
}

extern const unsigned VECTORS_IN_MATRIX;

Model::Model(   IDirect3DDevice9 *device, D3DPRIMITIVETYPE primitive_type, VertexShader &shader, unsigned vertex_size,
                const Vertex *vertices, unsigned vertices_count, const Index *indices, unsigned indices_count,
                unsigned primitives_count, D3DXVECTOR3 position, D3DXVECTOR3 rotation )
 
: device(device), vertices_count(vertices_count), primitives_count(primitives_count),
  primitive_type(primitive_type), vertex_buffer(NULL), index_buffer(NULL),
  position(position), rotation(rotation), vertex_size(vertex_size), shader(shader)
{
    _ASSERT(vertices != NULL);
    _ASSERT(indices != NULL);
    try
    {
        const unsigned vertices_size = vertices_count*vertex_size;
        const unsigned indices_size = indices_count*sizeof(indices[0]);

        if(FAILED( device->CreateVertexBuffer( vertices_size, D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &vertex_buffer, NULL ) ))
            throw VertexBufferInitError();
        

        if(FAILED( device->CreateIndexBuffer( indices_size, D3DUSAGE_WRITEONLY, INDEX_FORMAT, D3DPOOL_DEFAULT, &index_buffer, NULL ) ))
            throw IndexBufferInitError();

        // fill the vertex buffer.
        VOID* vertices_to_fill;
        if(FAILED( vertex_buffer->Lock( 0, vertices_size, &vertices_to_fill, 0 ) ))
            throw VertexBufferFillError();
        memcpy( vertices_to_fill, vertices, vertices_size );
        vertex_buffer->Unlock();

        // fill the index buffer.
        VOID* indices_to_fill;
        if(FAILED( index_buffer->Lock( 0, indices_size, &indices_to_fill, 0 ) ))
            throw IndexBufferFillError();
        memcpy( indices_to_fill, indices, indices_size );
        index_buffer->Unlock();
    
        update_matrix();
    }
    // using catch(...) because every caught exception is rethrown
    catch(...)
    {
        release_interfaces();
        throw;
    }
}

VertexShader &Model::get_shader()
{
    return shader;
}

void Model::draw() const
{
    check_render( device->SetStreamSource( 0, vertex_buffer, 0, vertex_size ) );
    check_render( device->SetIndices( index_buffer ) );
    check_render( device->DrawIndexedPrimitive( primitive_type, 0, 0, vertices_count, 0, primitives_count ) );
}

void Model::update_matrix()
{
    rotation_and_position = rotate_and_shift_matrix(rotation, position);
}

void Model::rotate(float phi)
{
    rotation.z += phi;
    update_matrix();
}

const D3DXMATRIX &Model::get_rotation_and_position() const
{
    return rotation_and_position;
}

void Model::release_interfaces()
{
    release_interface(vertex_buffer);
    release_interface(index_buffer);
}

Model::~Model()
{
    release_interfaces();
}

// -------------------------------------- SkinningModel -------------------------------------------------------------

SkinningModel::SkinningModel(IDirect3DDevice9 *device, D3DPRIMITIVETYPE primitive_type, VertexShader &shader,
                             const SkinningVertex *vertices, unsigned int vertices_count, const Index *indices, unsigned int indices_count,
                             unsigned int primitives_count, D3DXVECTOR3 position, D3DXVECTOR3 rotation, D3DXVECTOR3 bone_center)
: Model(device, primitive_type, shader, sizeof(SkinningVertex), vertices, vertices_count, indices, indices_count, primitives_count, position, rotation),
  bone_center(bone_center)
{
    _ASSERT( BONES_COUNT <= sizeof(D3DXVECTOR4) ); // to fit weights into vertex shader register
    for(unsigned i = 0; i < BONES_COUNT; ++i)
        bones[i] = rotate_x_matrix(0.0f);
}

void SkinningModel::set_time(float time)
{
    // first bone will set the rotation
    float angle = SKINNING_ANGLE*sin(SKINNING_OMEGA*time);
    bones[0] = rotate_x_matrix( angle, bone_center );
    // others will still be a unity matrix
}

unsigned SkinningModel::set_constants(D3DXVECTOR4 *out_data, unsigned buffer_size) const
// returns number of constants used
{
    _ASSERT( buffer_size >= BONES_COUNT*VECTORS_IN_MATRIX ); // enough space?
    memcpy(out_data, bones, BONES_COUNT*VECTORS_IN_MATRIX*sizeof(D3DXVECTOR4));
    return BONES_COUNT*VECTORS_IN_MATRIX;
}

// -------------------------------------- SkinningModel -------------------------------------------------------------

MorphingModel::MorphingModel(IDirect3DDevice9 *device, D3DPRIMITIVETYPE primitive_type, VertexShader &shader,
                             const Vertex *vertices, unsigned int vertices_count, const Index *indices, unsigned int indices_count,
                             unsigned int primitives_count, D3DXVECTOR3 position, D3DXVECTOR3 rotation, float final_radius)
: Model(device, primitive_type, shader, sizeof(Vertex), vertices, vertices_count, indices, indices_count, primitives_count, position, rotation),
  morphing_param(1), final_radius(final_radius)
{
}

void MorphingModel::set_time(float time)
{
    morphing_param = (cos(MORPHING_OMEGA*time) + 1.0f)/2.0f; // parameter of morhing: 0 to 1
}

unsigned MorphingModel::set_constants(D3DXVECTOR4 *out_data, unsigned buffer_size) const
// returns number of constants used
{
    _ASSERT( buffer_size >= MORPHING_CONSTANTS_USED); // enough space?
    out_data[0] = D3DXVECTOR4(final_radius, final_radius, final_radius, final_radius);
    out_data[1] = D3DXVECTOR4(morphing_param, morphing_param, morphing_param, morphing_param);
    return MORPHING_CONSTANTS_USED;
}
