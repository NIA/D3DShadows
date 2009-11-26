#include "tessellate.h"

const DWORD TESSELATE_DEGREE = 40;
const Index TESSELATED_VERTICES_COUNT = (TESSELATE_DEGREE+1)*(TESSELATE_DEGREE+2)/2;
const DWORD TESSELATED_INDICES_COUNT = 3*TESSELATE_DEGREE*TESSELATE_DEGREE;

namespace
{
    void add_triangle( Index i1, Index i2, Index i3, Index *indices, DWORD &current_index, Index offset )
    {
        indices[current_index++] = i1 + offset;
        indices[current_index++] = i2 + offset;
        indices[current_index++] = i3 + offset;
    }
}
void tessellate(const Vertex *src_vertices, const Index *src_indices, DWORD src_index_offset,
                Vertex *res_vertices, Index res_vertices_offset, Index *res_indices, D3DCOLOR color)
// Divides each side of triangle into given number of parts
// Writes data into arrays given as `res_vertices' and `res_indices',
//   assuming that there are already `res_vertices_offset' vertices before `res_vertices' pointer.
{
    _ASSERT(src_vertices != NULL);
    _ASSERT(src_indices != NULL);
    _ASSERT(res_vertices != NULL);
    _ASSERT(res_indices != NULL);
    // i1, i2 i3 are indices of source triangle vertices
    const Index i1 = src_indices[src_index_offset];
    const Index i2 = src_indices[src_index_offset + 1];
    const Index i3 = src_indices[src_index_offset + 2];
    const D3DXVECTOR3 step_down = (src_vertices[i1].pos - src_vertices[i2].pos)/TESSELATE_DEGREE;
    const D3DXVECTOR3 step_right = (src_vertices[i3].pos - src_vertices[i1].pos)/TESSELATE_DEGREE;
    D3DXVECTOR3 normal;
    D3DXVec3Cross(&normal, &step_down, &step_right);
    D3DXVec3Normalize(&normal, &normal);

    res_vertices[0] = src_vertices[i2];
    res_vertices[0].set_normal(normal);
    res_vertices[0].color = color;
    Index vertex = 1; // current vertex
    DWORD index = 0; // current index
    
    D3DXVECTOR3 start_pos = res_vertices[0].pos;
    for( Index line = 1; line <= TESSELATE_DEGREE; ++line )
    {
        for( Index column = 0; column < line + 1; ++column ) // line #1 contains 2 vertices
        {
            res_vertices[vertex] = Vertex( start_pos
                                             + static_cast<FLOAT>(line)*step_down
                                             + static_cast<FLOAT>(column)*step_right,
                                           color,
                                           normal);
            if( column != 0 ) // not first coumn
            {
                // add outer triangle
                add_triangle( vertex, vertex - 1, vertex - line - 1, res_indices, index, res_vertices_offset );
            }
            if( ( column != 0 ) && ( column != line ) ) // not first and not last column
            {
                // add inner triangle
                add_triangle(  vertex, vertex - line - 1, vertex - line, res_indices, index, res_vertices_offset );
            }
            ++vertex;
        }
    }
}
