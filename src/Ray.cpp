#include "Ray.h"

#include <cfloat>
#include <cmath>
#include <algorithm>

Ray::Ray( const vec3 & origin, const vec3 & direction )
    : m_Origin( origin ),
      m_Direction( direction ) {}

double Ray::triangleIntersection( Triangle & triangle, bool cullBackfaces ) {
    double infinity = std::numeric_limits<double>::infinity();
    double epsilon = 1e-9;

    vec3 e1 = triangle.B() - triangle.A();
    vec3 e2 = triangle.C() - triangle.A();
    vec3 p = vec3::cross( m_Direction, e2 );
    double d = vec3::dot( e1, p );

    if ( cullBackfaces ) {
        if ( d < epsilon ) return infinity;
    } else { 
        if ( fabs( d ) < epsilon ) return infinity;
    }

    double d_inv = 1.0 / d;
    vec3 q = m_Origin - triangle.A();
    double u = d_inv * vec3::dot( q, p );
    if ( u < 0.0 || u > 1.0 ) return infinity;

    vec3 r = vec3::cross( q, e1 );
    double v = d_inv * vec3::dot( r, m_Direction );
    if ( v < 0.0 || u + v > 1.0 ) return infinity;
    
    double t = d_inv * vec3::dot( e2, r );
    return t;
}

vec3 & Ray::o() {
    return m_Origin;
}

vec3 & Ray::s() {
    return m_Direction;
}

std::ostream & operator<<( std::ostream & os, Ray ray ) {
    os << ray.m_Origin << " + t" << ray.m_Direction;
    return os;
}