#include "main.h"
#include "Application.h"
#include "Model.h"
#include "cylinder.h"
#include "plane.h"
#include "pyramid.h"

namespace
{
    const char *SKINNING_SHADER_FILENAME = "skinning.vsh";
    const char *SKINNING_SHADOW_SHADER_FILENAME = "skinning_shadow.vsh";
    const char *MORPHING_SHADER_FILENAME = "morphing.vsh";
    const char *MORPHING_SHADOW_SHADER_FILENAME = "morphing_shadow.vsh";
    const char *PLANE_SHADER_FILENAME = "plane.vsh";
    const char *LIGHT_SOURCE_SHADER_FILENAME = "light_source.vsh";

    const D3DCOLOR colors[] =
    {
        D3DCOLOR_XRGB(250, 30, 10),
        D3DCOLOR_XRGB(250, 250, 0),
        D3DCOLOR_XRGB(30, 250, 0),
        D3DCOLOR_XRGB(0, 150, 250),
        D3DCOLOR_XRGB(250, 250, 250),
    };
    const unsigned colors_count = array_size(colors);
    const D3DCOLOR SPHERE_COLOR = D3DCOLOR_XRGB(255, 150, 0);
    const D3DCOLOR SECOND_CYLINDER_COLOR = D3DCOLOR_XRGB(0, 70, 220);
    const D3DCOLOR PLANE_COLOR = D3DCOLOR_XRGB(50,255,50);

    const float SPHERE_RADIUS = 0.7071f;
    const float LIGHT_SOURCE_RADIUS = 0.08f;

    const DWORD SPHERE_TESSELATE_DEGREE = 40;
    const Index SPHERE_ALL_TESSELATED_VERTICES_COUNT = pyramid_vertices_count(SPHERE_TESSELATE_DEGREE); // per 8 tessellated triangles
    const DWORD SPHERE_ALL_TESSELATED_INDICES_COUNT = pyramid_indices_count(SPHERE_TESSELATE_DEGREE); // per 8 tessellated triangles

    const DWORD LIGHT_SOURCE_TESSELATE_DEGREE = 10;
    const Index LIGHT_SOURCE_ALL_TESSELATED_VERTICES_COUNT = PLANES_PER_PYRAMID*tesselated_vertices_count(LIGHT_SOURCE_TESSELATE_DEGREE); // per 8 tessellated triangles
    const DWORD LIGHT_SOURCE_ALL_TESSELATED_INDICES_COUNT = PLANES_PER_PYRAMID*tesselated_indices_count(LIGHT_SOURCE_TESSELATE_DEGREE); // per 8 tessellated triangles

}

INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, INT )
{
    srand( static_cast<unsigned>( time(NULL) ) );
    
    SkinningVertex * cylinder_vertices = NULL;
    Index * cylinder_indices = NULL;
    Vertex * sphere_vertices = NULL;
    Index * sphere_indices = NULL;
    Vertex * plane_vertices = NULL;
    Index * plane_indices = NULL;
    Vertex * light_source_vertices = NULL;
    Index * light_source_indices = NULL;
    try
    {
        try
        {
            Application app;

            VertexShader skinning_shader(app.get_device(), SKINNING_VERTEX_DECL_ARRAY, SKINNING_SHADER_FILENAME);
            VertexShader skinning_shadow_shader(app.get_device(), SKINNING_VERTEX_DECL_ARRAY, SKINNING_SHADOW_SHADER_FILENAME);
            VertexShader morphing_shader(app.get_device(), VERTEX_DECL_ARRAY, MORPHING_SHADER_FILENAME);
            VertexShader morphing_shadow_shader(app.get_device(), VERTEX_DECL_ARRAY, MORPHING_SHADOW_SHADER_FILENAME);
            VertexShader plane_shader(app.get_device(), VERTEX_DECL_ARRAY, PLANE_SHADER_FILENAME);
            VertexShader light_source_shader(app.get_device(), VERTEX_DECL_ARRAY, LIGHT_SOURCE_SHADER_FILENAME);
            
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
            sphere_vertices = new Vertex[SPHERE_ALL_TESSELATED_VERTICES_COUNT];
            sphere_indices = new Index[SPHERE_ALL_TESSELATED_INDICES_COUNT];

            pyramid(SPHERE_RADIUS*SPHERE_RADIUS, sphere_vertices, sphere_indices, SPHERE_COLOR, SPHERE_TESSELATE_DEGREE);
            
            MorphingModel sphere( app.get_device(),
                                  D3DPT_TRIANGLELIST,
                                  morphing_shader,
                                  morphing_shadow_shader,
                                  sphere_vertices,
                                  SPHERE_ALL_TESSELATED_VERTICES_COUNT,
                                  sphere_indices,
                                  SPHERE_ALL_TESSELATED_INDICES_COUNT,
                                  SPHERE_ALL_TESSELATED_INDICES_COUNT/VERTICES_PER_TRIANGLE,
                                  D3DXVECTOR3(0, -1.3f, -0.2f),
                                  D3DXVECTOR3(0,0,0),
                                  SPHERE_RADIUS );

            // ----------------------------- P l a n e --------------------------
            plane_vertices = new Vertex[PLANE_VERTICES_COUNT];
            plane_indices = new Index[PLANE_INDICES_COUNT];
            plane(40, 40, plane_vertices, plane_indices, PLANE_COLOR);

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

            // -------------------------- Light source --------------------------
            light_source_vertices = new Vertex[LIGHT_SOURCE_ALL_TESSELATED_VERTICES_COUNT];
            light_source_indices = new Index[LIGHT_SOURCE_ALL_TESSELATED_INDICES_COUNT];

            pyramid(LIGHT_SOURCE_RADIUS*LIGHT_SOURCE_RADIUS, light_source_vertices, light_source_indices, D3DCOLOR_XRGB(0,0,0) /* ignored */, LIGHT_SOURCE_TESSELATE_DEGREE);

            LightSource light_source( app.get_device(),
                                      D3DPT_TRIANGLELIST,
                                      light_source_shader,
                                      light_source_vertices,
                                      LIGHT_SOURCE_ALL_TESSELATED_VERTICES_COUNT,
                                      light_source_indices,
                                      LIGHT_SOURCE_ALL_TESSELATED_INDICES_COUNT,
                                      LIGHT_SOURCE_ALL_TESSELATED_INDICES_COUNT/VERTICES_PER_TRIANGLE,
                                      D3DXVECTOR3(0,0,0),
                                      D3DXVECTOR3(0,0,0),
                                      LIGHT_SOURCE_RADIUS);

            // ---------------------------- a d d i n g -------------------------
            app.add_model(cylinder1);
            app.add_model(cylinder2);
            app.add_model(sphere);
            app.set_plane(plane);
            app.set_light_source_model(light_source);

            app.run();

            // -------------------------- c l e a n   u p -----------------------
            delete_array(&sphere_indices);
            delete_array(&sphere_vertices);
            delete_array(&cylinder_indices);
            delete_array(&cylinder_vertices);
            delete_array(&plane_indices);
            delete_array(&plane_vertices);
            delete_array(&light_source_indices);
            delete_array(&light_source_vertices);
        }
        catch(std::bad_alloc)
        {
            throw NoMemoryError();
        }
    }
    catch(RuntimeError &e)
    {
        delete_array(&sphere_indices);
        delete_array(&sphere_vertices);
        delete_array(&cylinder_indices);
        delete_array(&cylinder_vertices);
        delete_array(&plane_indices);
        delete_array(&plane_vertices);
        delete_array(&light_source_indices);
        delete_array(&light_source_vertices);
        const TCHAR *MESSAGE_BOX_TITLE = _T("Shadows error!");
        MessageBox(NULL, e.message(), MESSAGE_BOX_TITLE, MB_OK | MB_ICONERROR);
        return -1;
    }
    return 0;
}
