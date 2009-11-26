#pragma once
#include "main.h"
#include "Vertex.h"

extern const int PLANE_STEPS_PER_HALF_SIDE;
extern const Index PLANE_VERTICES_COUNT;
extern const DWORD PLANE_INDICES_COUNT;

void plane(float length, float width, Vertex *res_vertices, Index *res_indices, D3DCOLOR color);
