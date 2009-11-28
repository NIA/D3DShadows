#include "Application.h"
#include <time.h>

const unsigned VECTORS_IN_MATRIX = sizeof(D3DXMATRIX)/sizeof(D3DXVECTOR4);

namespace
{
    const int         WINDOW_SIZE = 600;
    const D3DCOLOR    BACKGROUND_COLOR = D3DCOLOR_XRGB( 15, 15, 25 );
    const bool        INITIAL_WIREFRAME_STATE = false;
    const D3DCOLOR    BLACK = D3DCOLOR_XRGB( 0, 0, 0 );
    const float       ROTATE_STEP = D3DX_PI/30.0f;
    const float       POINT_MOVING_STEP = 0.03f;
    const char       *SHADOW_SHADER_FILENAME = "shadow.vsh";
    const DWORD       STENCIL_REF_VALUE = 50;


    //---------------- SHADER CONSTANTS ---------------------------
    //    c0-c3 is the view matrix
    const unsigned    SHADER_REG_VIEW_MX = 0;
    //    c4-c7 is the first bone matrix for SKINNING
    //    c8-c11 is the second bone matrix for SKINNING
    //    c4 is final radius for MORPHING
    //    c5 is MORPHING parameter
    const unsigned    SHADER_REG_MODEL_DATA = 4;
    const unsigned    SHADER_SPACE_MODEL_DATA = 8; // number of registers available for
    //    c14 is diffuse coefficient
    const unsigned    SHADER_REG_DIFFUSE_COEF = 14;
    const float       SHADER_VAL_DIFFUSE_COEF = 0.7f;
    //    c15 is ambient color
    const unsigned    SHADER_REG_AMBIENT_COLOR = 15;
    const D3DCOLOR    SHADER_VAL_AMBIENT_COLOR = D3DCOLOR_XRGB(20, 20, 20);
    //    c16 is point light color
    const unsigned    SHADER_REG_POINT_COLOR = 16;
    const D3DCOLOR    SHADER_VAL_POINT_COLOR = D3DCOLOR_XRGB(204, 204, 100);
    //    c17 is point light position
    const unsigned    SHADER_REG_POINT_POSITION = 17;
    const D3DXVECTOR3 SHADER_VAL_POINT_POSITION  (0.2f, -0.91f, 1.5f);
    //    c18 are attenuation constants
    const unsigned    SHADER_REG_ATTENUATION = 18;
    const D3DXVECTOR3 SHADER_VAL_ATTENUATION  (1.0f, 0, 0.5f);
    //    c19 is specular coefficient
    const unsigned    SHADER_REG_SPECULAR_COEF = 19;
    const float       SHADER_VAL_SPECULAR_COEF = 0.4f;
    //    c20 is specular constant 'f'
    const unsigned    SHADER_REG_SPECULAR_F = 20;
    const float       SHADER_VAL_SPECULAR_F = 35.0f;
    //    c21 is eye position
    const unsigned    SHADER_REG_EYE = 21;
    //    c27-c30 is position and rotation of model matrix
    const unsigned    SHADER_REG_POS_AND_ROT_MX = 27;
    //    c31-c34 is shadow projection matrix
    const unsigned    SHADER_REG_SHADOW_PROJ_MX = 31;
    //    c35 are attenuation constants
    const unsigned    SHADER_REG_SHADOW_ATTENUATION = 35;
    const D3DXVECTOR3 SHADER_VAL_SHADOW_ATTENUATION  (0.8f, 0, 0.1f);
}

Application::Application() :
    d3d(NULL), device(NULL), window(WINDOW_SIZE, WINDOW_SIZE), camera(5, 0.48f, 0), // Constants selected for better view of the scene
    point_light_enabled(true), ambient_light_enabled(true), plane(NULL), point_light_position(SHADER_VAL_POINT_POSITION)
{
    try
    {
        init_device();
    }
    // using catch(...) because every caught exception is rethrown
    catch(...)
    {
        release_interfaces();
        throw;
    }
}

void Application::init_device()
{
    d3d = Direct3DCreate9( D3D_SDK_VERSION );
    if( d3d == NULL )
        throw D3DInitError();

    // Set up the structure used to create the device
    D3DPRESENT_PARAMETERS present_parameters;
    ZeroMemory( &present_parameters, sizeof( present_parameters ) );
    present_parameters.Windowed = TRUE;
    present_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    present_parameters.BackBufferFormat = D3DFMT_UNKNOWN;
    present_parameters.EnableAutoDepthStencil = TRUE;
    present_parameters.AutoDepthStencilFormat = D3DFMT_D24S8;
    // Create the device
    if( FAILED( d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window,
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &present_parameters, &device ) ) )
        throw D3DInitError();
    check_state( device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ) );
    // Enable stencil testing
    check_state( device->SetRenderState(D3DRS_STENCILENABLE, TRUE) );
    // Set the comparison reference value
    check_state( device->SetRenderState(D3DRS_STENCILREF, STENCIL_REF_VALUE) );
    //  Specify a stencil mask 
    device->SetRenderState(D3DRS_STENCILMASK, 0xff);
    //  Configure alpha-blending
    check_state( device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA) );
    check_state( device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA) );

    toggle_wireframe();
}

inline void Application::draw_model(Model *model, float time, bool shadow)
{
    static D3DXVECTOR4 model_constants[SHADER_SPACE_MODEL_DATA];
    static unsigned constants_used;


    // Setting constants
    model->set_time( time );
    constants_used = model->set_constants( model_constants, array_size(model_constants) );
    set_shader_const( SHADER_REG_MODEL_DATA, *model_constants, constants_used );
    set_shader_matrix( SHADER_REG_POS_AND_ROT_MX, model->get_rotation_and_position() );
    
    // Draw
    if( shadow )
    {
        // Shadow
        ( model->get_shadow_shader() ).set();
    }
    else
    {
        ( model->get_shader() ).set();
    }
    model->draw();
}

void Application::render()
{
    check_render( device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, BACKGROUND_COLOR, 1.0f, 0 ) );
    
    // Begin the scene
    check_render( device->BeginScene() );
    // Setting constants
    float time = static_cast<float>( clock() )/static_cast<float>( CLOCKS_PER_SEC );

    D3DCOLOR ambient_color = ambient_light_enabled ? SHADER_VAL_AMBIENT_COLOR : BLACK;
    D3DCOLOR point_color = point_light_enabled ? SHADER_VAL_POINT_COLOR : BLACK;
    D3DXMATRIX shadow_proj_matrix = plane->get_projection_matrix(point_light_position);

    set_shader_matrix( SHADER_REG_VIEW_MX,        camera.get_matrix()       );
    set_shader_float ( SHADER_REG_DIFFUSE_COEF,   SHADER_VAL_DIFFUSE_COEF   );
    set_shader_color ( SHADER_REG_AMBIENT_COLOR,  ambient_color             );
    set_shader_color ( SHADER_REG_POINT_COLOR,    point_color               );
    set_shader_point ( SHADER_REG_POINT_POSITION, point_light_position      );
    set_shader_vector( SHADER_REG_ATTENUATION,    SHADER_VAL_ATTENUATION    );
    set_shader_float ( SHADER_REG_SPECULAR_COEF,  SHADER_VAL_SPECULAR_COEF  );
    set_shader_float ( SHADER_REG_SPECULAR_F,     SHADER_VAL_SPECULAR_F     );
    set_shader_point ( SHADER_REG_EYE,            camera.get_eye()          );
    set_shader_matrix( SHADER_REG_SHADOW_PROJ_MX, shadow_proj_matrix        );
    set_shader_vector( SHADER_REG_SHADOW_ATTENUATION, SHADER_VAL_SHADOW_ATTENUATION );

    // Draw Plane
    check_state( device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE) );
    draw_model( plane, time, false );
    // Draw shadows if point_light_enabled
    check_state( device->SetRenderState(D3DRS_ZENABLE, FALSE) );
    check_state( device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL) );
    check_state( device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCRSAT) );
    check_state( device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE) );
    if ( point_light_enabled )
    {
        for ( Models::iterator iter = models.begin(); iter != models.end(); ++iter )
        {
            draw_model( *iter, time, true );
        }
    }
    // Draw models
    check_state( device->SetRenderState(D3DRS_ZENABLE, TRUE) );
    check_state( device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS) );
    check_state( device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE) );
    for ( Models::iterator iter = models.begin(); iter != models.end(); ++iter )
    {
        draw_model( *iter, time, false );
    }
    // End the scene
    check_render( device->EndScene() );
    
    // Present the backbuffer contents to the display
    check_render( device->Present( NULL, NULL, NULL, NULL ) );

}

IDirect3DDevice9 * Application::get_device()
{
    return device;
}

void Application::add_model(Model &model)
{
    models.push_back( &model );
}

void Application::remove_model(Model &model)
{
    models.remove( &model );
}

void Application::rotate_models(float phi)
{
    for ( Models::iterator iter = models.begin(); iter != models.end(); ++iter )
    {
        (*iter)->rotate(phi);
    }
}

void Application::process_key(unsigned code)
{
    switch( code )
    {
    case VK_ESCAPE:
        PostQuitMessage( 0 );
        break;
    case VK_UP:
        camera.move_up();
        break;
    case VK_DOWN:
        camera.move_down();
        break;
    case VK_PRIOR:
    case VK_ADD:
    case VK_OEM_PLUS:
        camera.move_nearer();
        break;
    case VK_NEXT:
    case VK_SUBTRACT:
    case VK_OEM_MINUS:
        camera.move_farther();
        break;
    case VK_LEFT:
        camera.move_clockwise();
        break;
    case VK_RIGHT:
        camera.move_counterclockwise();
        break;
    case 'A':
        point_light_position.y += POINT_MOVING_STEP;
        break;
    case 'D':
        point_light_position.y -= POINT_MOVING_STEP;
        break;
    case 'W':
        point_light_position.x -= POINT_MOVING_STEP;
        break;
    case 'S':
        point_light_position.x += POINT_MOVING_STEP;
        break;
    case 'R':
        point_light_position.z += POINT_MOVING_STEP;
        break;
    case 'F':
        point_light_position.z -= POINT_MOVING_STEP;
        break;
    case '1':
        point_light_enabled = !point_light_enabled;
        break;
    case '2':
        ambient_light_enabled = !ambient_light_enabled;
        break;
    }
}

void Application::run()
{
    if( plane == NULL )
        throw NoPlaneError();

    window.show();
    window.update();

    // Enter the message loop
    MSG msg;
    ZeroMemory( &msg, sizeof( msg ) );
    while( msg.message != WM_QUIT )
    {
        if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
            if( msg.message == WM_KEYDOWN )
            {
                process_key( static_cast<unsigned>( msg.wParam ) );
            }

            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
            render();
    }
}

void Application::toggle_wireframe()
{
    static bool wireframe = !INITIAL_WIREFRAME_STATE;
    wireframe = !wireframe;

    if( wireframe )
    {
        check_state( device->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME ) );
    }
    else
    {
        check_state( device->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID ) );
    }
}

void Application::release_interfaces()
{
    release_interface( d3d );
    release_interface( device );
}

Application::~Application()
{
    release_interfaces();
}