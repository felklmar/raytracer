#pragma once

#include "vec3.h"

class Camera {
    private:
        vec3 m_Position;
        vec3 m_Up;
        vec3 m_Direction;
        double m_FOV;

        vec3 readVector( const std::string & line );
    public:
        Camera( const std::string & filename );

        vec3 getPosition();
        vec3 getUp();
        vec3 getDirection();
        double getFOV();

        friend std::ostream & operator<<( std::ostream & os, Camera c );
};