#pragma once

#include "vec3.h"
#include "Triangle.h"

#include <vector>

class Ray {
    private:
        vec3 m_Origin;
        vec3 m_Direction;
    public:
        Ray( const vec3 & origin, const vec3 & direction );
        double triangleIntersection( Triangle & triangle, bool cullBackfaces = true );
        vec3 & o();
        vec3 & s();
        friend std::ostream & operator<<( std::ostream & os, Ray ray );
};