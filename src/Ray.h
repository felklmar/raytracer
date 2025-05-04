#pragma once

#include "vec3.h"
#include "IntersectionData.h"

#include <vector>

class Triangle;

class Ray {
    private:
        vec3 m_Origin;
        vec3 m_Direction;
    public:
        Ray( const vec3 & origin, const vec3 & direction );
        IntersectionData triangleIntersection( const Triangle & triangle, bool cullBackfaces = true ) const; 

        vec3 getOrigin() const;
        vec3 getDirection() const;

        friend std::ostream & operator<<( std::ostream & os, Ray ray );
};