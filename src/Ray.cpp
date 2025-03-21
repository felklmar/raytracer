#include "Ray.h"

#include <cfloat>
#include <cmath>
#include <algorithm>

Ray::Ray( const vec3 & origin, const vec3 & direction )
    : m_Origin( origin ),
      m_Direction( direction ) {}

double Ray::triangleIntersection( const Triangle & triangle, bool cullBackfaces ) const {
    double infinity = std::numeric_limits<double>::infinity();
    double epsilon = std::numeric_limits<double>::epsilon();

    vec3 e1 = triangle.getB() - triangle.getA();
    vec3 e2 = triangle.getC() - triangle.getA();
    vec3 p = vec3::cross( m_Direction, e2 );
    double d = vec3::dot( e1, p );

    if ( cullBackfaces ) {
        if ( d < epsilon ) return infinity;
    } else { 
        if ( fabs( d ) < epsilon ) return infinity;
    }

    double d_inv = 1.0 / d;
    vec3 q = m_Origin - triangle.getA();
    double u = d_inv * vec3::dot( q, p );
    if ( u < 0.0 || u > 1.0 ) return infinity;

    vec3 r = vec3::cross( q, e1 );
    double v = d_inv * vec3::dot( r, m_Direction );
    if ( v < 0.0 || u + v > 1.0 ) return infinity;
    
    return d_inv * vec3::dot( e2, r );
}

const vec3 & Ray::getOrigin() const {
    return m_Origin;
}

const vec3 & Ray::getDirection() const{
    return m_Direction;
}
        
std::ostream & operator<<( std::ostream & os, Ray ray ) {
    os << ray.m_Origin << " + t" << ray.m_Direction;
    return os;
}