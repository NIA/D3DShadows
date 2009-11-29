#pragma once
#include "main.h"
#include "Vertex.h"

inline Index tesselated_vertices_count(DWORD tessellate_degree)
{
    return (tessellate_degree+1)*(tessellate_degree+2)/2;
}
inline DWORD tesselated_indices_count(DWORD tessellate_degree)
{
    return 3*tessellate_degree*tessellate_degree;
}

// Divides each side of triangle into given number of parts
// Writes data into arrays given as `res_vertices' and `res_indices',
//   assuming that there are already `res_vertices_offset' vertices before `res_vertices' pointer.
void tessellate(const Vertex *src_vertices, const Index *src_indices, DWORD src_index_offset,
                Vertex *res_vertices, Index res_vertices_offset, Index *res_indices, D3DCOLOR color, DWORD tesselate_degree);
