#include "main.h"
#include "Application.h"
#include "Model.h"
#include "cylinder.h"
#include "tessellate.h"
#include "plane.h"

namespace
{
    const char *SKINNING_SHADER_FILENAME = "skinning.vsh";
    const char *SKINNING_SHADOW_SHADER_FILENAME = "skinning_shadow.vsh";
    const char *MORPHING_SHADER_FILENAME = "morphing.vsh";
    const char *MORPHING_SHADOW_SHADER_FILENAME = "morphing_shadow.vsh";
    const char *PLANE_SHADER_FILENAME = "plane.vsh";
    const D3DCOLOR colors[] =
    {
        D3DCOLOR_XRGB(250, 250, 250),
        D3DCOLOR_XRGB(250, 30, 10),
        D3DCOLOR_XRGB(250, 250, 0),
        D3DCOLOR_XRGB(30, 250, 0),
        D3DCOLOR_XRGB(0, 150, 250),
    };
    const unsigned colors_count = array_size(colors);
    const D3DCOLOR SPHERE_COLOR = D3DCOLOR_XRGB(255, 150, 0);
    const D3DCOLOR SECOND_CYLINDER_COLOR = D3DCOLOR_XRGB(30, 250, 0);
    const D3DCOLOR PLANE_COLOR = D3DCOLOR_XRGB(255,255,255);
}

INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, INT )
{
    srand( static_cast<unsigned>( time(NULL) ) );
    
    SkinningVertex * cylinder_vertices = NULL;
    Index * cylinder_indices = NULL;
    Vertex * tesselated_vertices = NULL;
    Index * tesselated_indices = NULL;
    Vertex * plane_vertices = NULL;
    Index * plane_indices = NULL;
    try
    {
        Application app;

        VertexShader skinning_shader(app.get_device(), SKINNING_VERTEX_DECL_ARRAY, SKINNING_SHADER_FILENAME);
        VertexShader skinning_shadow_shader(app.get_device(), SKINNING_VERTEX_DECL_ARRAY, SKINNING_SHADOW_SHADER_FILENAME);
        VertexShader morphing_shader(app.get_device(), VERTEX_DECL_ARRAY, MORPHING_SHADER_FILENAME);
        VertexShader morphing_shadow_shader(app.get_device(), VERTEX_DECL_ARRAY, MORPHING_SHADOW_SHADER_FILENAME);
        VertexShader plane_shader(app.get_device(), VERTEX_DECL_ARRAY, PLANE_SHADER_FILENAME);
        
        // -------------------------- C y l i n d e r -----------------------

        cylinder_vertices = new SkinningVertex[CYLINDER_VERTICES_COUNT];
        cylinder_indices = new Index[CYLINDER_INDICES_COUNT];

        float height = 2.0f;
        cylinder( 0.7f, height,
                  colors, colors_count,
                  cylinder_vertices, cylinder_indices );

        SkinningModel cylinder1(app.get_device(),
                                D3DPT_TRIANGLESTRIP,
                                skinning_shader,
                                skinning_shadow_shader,
                                cylinder_vertices,
                                CYLINDER_VERTICES_COUNT,
                                cylinder_indices,
                                CYLINDER_INDICES_COUNT,
                                CYLINDER_INDICES_COUNT - 2,
                                D3DXVECTOR3(0.5f, 0.5f, -height/2),
                                D3DXVECTOR3(0,0,0),
                                D3DXVECTOR3(0,0,-1));

        height = 2.3f;
        cylinder( 0.3f, height,
                  &SECOND_CYLINDER_COLOR, 1,
                  cylinder_vertices, cylinder_indices );

        SkinningModel cylinder2(app.get_device(),
                                D3DPT_TRIANGLESTRIP,
                                skinning_shader,
                                skinning_shadow_shader,
                                cylinder_vertices,
                                CYLINDER_VERTICES_COUNT,
                                cylinder_indices,
                                CYLINDER_INDICES_COUNT,
                                CYLINDER_INDICES_COUNT - 2,
                                D3DXVECTOR3(-1.0f, 0.5f, height/2),
                                D3DXVECTOR3(D3DX_PI,0,-D3DX_PI/4),
                                D3DXVECTOR3(0,0,1));

        
        // -------------------------- P y r a m i d -----------------------
        const Index PLANES_PER_PYRAMID = 8;
        const D3DXVECTOR3 normal_up(0,0,1);
        const Vertex pyramid_vertices[]=
        {
            Vertex(D3DXVECTOR3(  0.5f, -0.5f,  0.00f ),normal_up),
            Vertex(D3DXVECTOR3( -0.5f, -0.5f,  0.00f ),normal_up),
            Vertex(D3DXVECTOR3( -0.5f,  0.5f,  0.00f ),normal_up),
            Vertex(D3DXVECTOR3(  0.5f,  0.5f,  0.00f ),normal_up),
            Vertex(D3DXVECTOR3(  0.0f,  0.0f,  0.7071f ),normal_up),
            Vertex(D3DXVECTOR3(  0.0f,  0.0f, -0.7071f ),normal_up),
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

        const Index ALL_TESSELATED_VERTICES_COUNT = PLANES_PER_PYRAMID*TESSELATED_VERTICES_COUNT; // per 8 tessellated triangles
        const DWORD ALL_TESSELATED_INDICES_COUNT = PLANES_PER_PYRAMID*TESSELATED_INDICES_COUNT; // per 8 tessellated triangles

        tesselated_vertices = new Vertex[ALL_TESSELATED_VERTICES_COUNT];
        tesselated_indices = new Index[ALL_TESSELATED_INDICES_COUNT];

        for( DWORD i = 0; i < PLANES_PER_PYRAMID; ++i )
        {
            tessellate( pyramid_vertices, pyramid_indices, i*VERTICES_PER_TRIANGLE,
                        &tesselated_vertices[i*TESSELATED_VERTICES_COUNT], i*TESSELATED_VERTICES_COUNT,
                        &tesselated_indices[i*TESSELATED_INDICES_COUNT], SPHERE_COLOR );
        }
        
        MorphingModel pyramid( app.get_device(),
                               D3DPT_TRIANGLELIST,
                               morphing_shader,
                               morphing_shadow_shader,
                               tesselated_vertices,
                               ALL_TESSELATED_VERTICES_COUNT,
                               tesselated_indices,
                               ALL_TESSELATED_INDICES_COUNT,
                               ALL_TESSELATED_INDICES_COUNT/VERTICES_PER_TRIANGLE,
                               D3DXVECTOR3(0, -1.3f, -0.2f),
                               D3DXVECTOR3(0,0,0),
                               0.7071f );

        // ----------------------------- P l a n e --------------------------
        plane_vertices = new Vertex[PLANE_VERTICES_COUNT];
        plane_indices = new Index[PLANE_INDICES_COUNT];
        plane(7, 7, plane_vertices, plane_indices, PLANE_COLOR);

        Plane plane( app.get_device(),
                     D3DPT_TRIANGLELIST,
                     plane_shader,
                     plane_vertices,
                     PLANE_VERTICES_COUNT,
                     plane_indices,
                     PLANE_INDICES_COUNT,
                     PLANE_INDICES_COUNT/VERTICES_PER_TRIANGLE,
                     D3DXVECTOR3(0, 0, -1.2f),
                     D3DXVECTOR3(0,0,0) );

        // ---------------------------- a d d i n g -------------------------
        app.add_model(cylinder1);
        app.add_model(cylinder2);
        app.add_model(pyramid);
        app.set_plane(plane);

        app.run();

        // -------------------------- c l e a n   u p -----------------------
        delete_array(&tesselated_indices);
        delete_array(&tesselated_vertices);
        delete_array(&cylinder_indices);
        delete_array(&cylinder_vertices);
        delete_array(&plane_indices);
        delete_array(&plane_vertices);
    }
    catch(RuntimeError &e)
    {
        delete_array(&tesselated_indices);
        delete_array(&tesselated_vertices);
        delete_array(&cylinder_indices);
        delete_array(&cylinder_vertices);
        delete_array(&plane_indices);
        delete_array(&plane_vertices);
        const TCHAR *MESSAGE_BOX_TITLE = _T("Shadows error!");
        MessageBox(NULL, e.message(), MESSAGE_BOX_TITLE, MB_OK | MB_ICONERROR);
        return -1;
    }
    return 0;
}
