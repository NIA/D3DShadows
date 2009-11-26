#pragma once
#include "main.h"
#include "Vertex.h"

extern const Index CYLINDER_EDGES_PER_BASE;
extern const Index CYLINDER_EDGES_PER_HEIGHT;
extern const Index CYLINDER_EDGES_PER_CAP;

extern const Index CYLINDER_VERTICES_COUNT;
extern const DWORD CYLINDER_INDICES_COUNT; // Calculated for TRIANGLESTRIP primitive type

// Writes data into arrays given as `res_vertices' and `res_indices',
void cylinder( float radius, float height,
               const D3DCOLOR *colors, unsigned colors_count,
               SkinningVertex *res_vertices, Index *res_indices);
