#pragma once

#include "vec3.h"

class Camera {
    private:
        vec3 m_Position;
        vec3 m_Up;
        vec3 m_Direction;
        double m_FOV;
    public:
        Camera() = default;
        Camera( const vec3 & position, const vec3 & up, const vec3 & direction, double fov );

        vec3 getPosition();
        vec3 getUp();
        vec3 getDirection();
        double getFOV();

        friend std::ostream & operator<<( std::ostream & os, Camera c );
}; 