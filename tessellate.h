#pragma once
#include "main.h"
#include "Vertex.h"

extern const DWORD TESSELATE_DEGREE;
extern const Index TESSELATED_VERTICES_COUNT;
extern const DWORD TESSELATED_INDICES_COUNT;

// Divides each side of triangle into given number of parts
// Writes data into arrays given as `res_vertices' and `res_indices',
//   assuming that there are already `res_vertices_offset' vertices before `res_vertices' pointer.
void tessellate(const Vertex *src_vertices, const Index *src_indices, DWORD src_index_offset,
                Vertex *res_vertices, Index res_vertices_offset, Index *res_indices, D3DCOLOR color);
