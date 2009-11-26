#include "Camera.h"

namespace
{
    const float NEAR_CLIP = 0.8f;
    const float FAR_CLIP = 1e10f;
    const float COEFF_A = FAR_CLIP / (FAR_CLIP - NEAR_CLIP);
    const float COEFF_B = - COEFF_A * NEAR_CLIP;

    const D3DXMATRIX PROJ_MX ( NEAR_CLIP,         0,       0,       0,
                                       0, NEAR_CLIP,       0,       0,
                                       0,         0, COEFF_A, COEFF_B,
                                       0,         0,       1,       0 );

    const float RHO_STEP = 0.1f;
    const float THETA_STEP = D3DX_PI/40.0f;
    const float PHI_STEP = D3DX_PI/30.0f;

    const float RHO_MIN = 1.74f + NEAR_CLIP; // Suppose models are inside the cube 1x1x1 => in sphere with radius sqrt(3) (approx. 1,74).
    const float THETA_MIN = 0.01f;

    const float RHO_MAX = FAR_CLIP;
    const float THETA_MAX = D3DX_PI - 0.01f; // A little subtraction - to prevent looking parallel to the 'up' vector

    const D3DXVECTOR3 DEFAULT_POSITION(1.5f*RHO_MIN, D3DX_PI/3.0f, D3DX_PI/3.0f);
}

Camera::Camera()
{
    set(DEFAULT_POSITION.x, DEFAULT_POSITION.y, DEFAULT_POSITION.z, 0, 0, 0);
}

void Camera::set_position(float rho, float theta, float phi, bool update_mx)
{
    eye_spheric = D3DXVECTOR3(rho, theta, phi);
    if( update_mx )
        update_matrices();
}

void Camera::set_at_position(float x, float y, float z, bool update_mx)
{
    at = D3DXVECTOR3(x, y, z);
    if( update_mx )
        update_matrices();
}

void Camera::set_up_direction(float x, float y, float z, bool update_mx)
{
    up = D3DXVECTOR3(x, y, z);
    if( update_mx )
        update_matrices();
}

void Camera::check_coord_bounds()
{
    if (eye_spheric.x < RHO_MIN)
    {
        eye_spheric.x = RHO_MIN;
    }
    if (eye_spheric.x > RHO_MAX)
    {
        eye_spheric.x = RHO_MAX;
    }
    if (eye_spheric.y < THETA_MIN)
    {
        eye_spheric.y = THETA_MIN;
    }
    if (eye_spheric.y > THETA_MAX)
    {
        eye_spheric.y = THETA_MAX;
    }
}

void Camera::change_rho(float addition)
{
    eye_spheric.x += addition;
    check_coord_bounds();
    update_matrices();
}

void Camera::change_theta(float addition)
{
    eye_spheric.y += addition;
    check_coord_bounds();
    update_matrices();
}

void Camera::change_phi(float addition)
{
    eye_spheric.z += addition;
    check_coord_bounds();
    update_matrices();
}

void Camera::move_nearer()
{
    change_rho( -RHO_STEP );
}
void Camera::move_farther()
{
    change_rho( RHO_STEP );
}
void Camera::move_up()
{
    change_theta( -THETA_STEP );
}
void Camera::move_down()
{
    change_theta( THETA_STEP );
}
void Camera::move_clockwise()
{
    change_phi( PHI_STEP );
}
void Camera::move_counterclockwise()
{
    change_phi( -PHI_STEP );
}

static D3DXVECTOR3 spheric_to_cartesian( D3DXVECTOR3 spheric )
{
    float rho = spheric.x;
    float theta = spheric.y;
    float phi = spheric.z;

    return D3DXVECTOR3( rho*sin(theta)*cos(phi), rho*sin(theta)*sin(phi), rho*cos(theta) );
}

void Camera::update_matrices()
{
    D3DXVECTOR3 eye = get_eye();
    D3DXVECTOR3 axis_x, axis_y, axis_z;
    axis_z = at - eye;
    D3DXVec3Cross( &axis_x, &up, &axis_z );
    D3DXVec3Cross( &axis_y, &axis_z, &axis_x );

    D3DXVec3Normalize( &axis_x, &axis_x );
    D3DXVec3Normalize( &axis_y, &axis_y );
    D3DXVec3Normalize( &axis_z, &axis_z );

    D3DXVECTOR3 d( - D3DXVec3Dot( &eye, &axis_x ),
                   - D3DXVec3Dot( &eye, &axis_y ),
                   - D3DXVec3Dot( &eye, &axis_z ) );

    D3DXMATRIX view_mx( axis_x.x, axis_x.y, axis_x.z, d.x,
                        axis_y.x, axis_y.y, axis_y.z, d.y,
                        axis_z.x, axis_z.y, axis_z.z, d.z,
                               0,        0,        0,   1 );
    mx = PROJ_MX * view_mx;
}

D3DXMATRIX Camera::get_matrix() const
{
    return mx;
}

D3DXVECTOR3 Camera::get_eye() const
{
    return spheric_to_cartesian( eye_spheric );
}
