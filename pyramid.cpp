#include "pyramid.h"

void pyramid( float side, Vertex *res_vertices, Index *res_indices,
              D3DCOLOR color, DWORD tesselate_degree )
{
    const D3DXVECTOR3 normal_up(0,0,1);
    const Vertex pyramid_vertices[]=
    {
        Vertex(D3DXVECTOR3(  side, -side,  0.00f ),normal_up),
        Vertex(D3DXVECTOR3( -side, -side,  0.00f ),normal_up),
        Vertex(D3DXVECTOR3( -side,  side,  0.00f ),normal_up),
        Vertex(D3DXVECTOR3(  side,  side,  0.00f ),normal_up),
        Vertex(D3DXVECTOR3(  0.0f,  0.0f,  sqrt(side) ),normal_up),
        Vertex(D3DXVECTOR3(  0.0f,  0.0f, -sqrt(side) ),normal_up),
    };
    const Index pyramid_indices[PLANES_PER_PYRAMID*VERTICES_PER_TRIANGLE] =
    {
        0, 4, 3,
        3, 4, 2,
        2, 4, 1,
        1, 4, 0,

        0, 3, 5,
        3, 2, 5,
        2, 1, 5,
        1, 0, 5,
    };

    for( DWORD i = 0; i < PLANES_PER_PYRAMID; ++i )
    {
        tessellate( pyramid_vertices, pyramid_indices, i*VERTICES_PER_TRIANGLE,
                    &res_vertices[i*tesselated_vertices_count(tesselate_degree)], i*tesselated_vertices_count(tesselate_degree),
                    &res_indices[i*tesselated_indices_count(tesselate_degree)], color, tesselate_degree );
    }
}