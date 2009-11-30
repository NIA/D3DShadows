#include "plane.h"

const int PLANE_STEPS_PER_HALF_SIDE = 300;
const Index PLANE_VERTICES_COUNT = (2*PLANE_STEPS_PER_HALF_SIDE + 1)*(2*PLANE_STEPS_PER_HALF_SIDE + 1);
const DWORD PLANE_INDICES_COUNT = 2*VERTICES_PER_TRIANGLE*(2*PLANE_STEPS_PER_HALF_SIDE)*(2*PLANE_STEPS_PER_HALF_SIDE);

void plane(float length, float width, Vertex *res_vertices, Index *res_indices, D3DCOLOR color)
{
    Index vertex = 0; // current vertex
    DWORD index = 0; // current index
    _ASSERT(PLANE_STEPS_PER_HALF_SIDE != 0);

    const float x_step = length/(2*PLANE_STEPS_PER_HALF_SIDE);
    const float y_step = width/(2*PLANE_STEPS_PER_HALF_SIDE);
    const Index vertices_in_line = (2*PLANE_STEPS_PER_HALF_SIDE + 1);
    D3DXVECTOR3 normal(0, 0, 1);

    for( int i = -PLANE_STEPS_PER_HALF_SIDE; i <= PLANE_STEPS_PER_HALF_SIDE; ++i )
    {
        for( int j = -PLANE_STEPS_PER_HALF_SIDE; j <= PLANE_STEPS_PER_HALF_SIDE; ++j )
        {
            res_vertices[vertex] = Vertex( D3DXVECTOR3( x_step*i, y_step*j, 0), color, normal);
            if( i != -PLANE_STEPS_PER_HALF_SIDE && j != -PLANE_STEPS_PER_HALF_SIDE)
            {
                // if not first line and column
                add_triangle(vertex, vertex-1, vertex-1-vertices_in_line, res_indices, index);
                add_triangle(vertex, vertex-1-vertices_in_line, vertex-vertices_in_line, res_indices, index);
            }
            ++vertex;
        }
    }
}
