#include "vec3.h"

#include <cmath>

vec3::vec3() : x( 0 ), y( 0 ), z( 0 ) {}
vec3::vec3( double v ) : x( v ), y( v ), z( v ) {}
vec3::vec3( double x0, double y0, double z0 ) : x( x0 ), y( y0 ), z( z0 ) {}

vec3 vec3::operator*( double a ) const {
    return vec3( x * a, y * a, z * a );
}

vec3 vec3::operator*( const vec3 r ) const {
    return vec3( x * r.x, y * r.y, z * r.z );
}

vec3 vec3::operator/( const double r ) const { 
    return std::fabs( r ) > epsilon ? vec3( x / r, y / r, z / r ) : vec3( 0, 0, 0 ); 
}

vec3 vec3::operator+( const vec3 & v ) const { 
    return vec3( x + v.x, y + v.y, z + v.z ); 
}

vec3 vec3::operator-( const vec3 & v ) const { 
    return vec3( x - v.x, y - v.y, z - v.z ); 
}

vec3 vec3::operator-() const { 
    return vec3( -x, -y, -z ); 
}

void vec3::operator+=( const vec3 & v ) { 
    x += v.x; y += v.y; z += v.z; 
}

void vec3::operator*=( double a ) {
    x *= a; y *= a; z *= a; 
}

void vec3::operator*=( const vec3 & v ) {
    x *= v.x; y *= v.y; z *= v.z; 
}

double vec3::length() const {
    return std::sqrt( x * x + y * y + z * z ); 
}

double vec3::average() {
    return ( x + y + z ) / 3; 
}

vec3 vec3::normalize() const { 
    return ( *this ) / length(); 
}

double vec3::dot( const vec3 & other ) const {
    return dot( *this, other ); 
}

vec3 vec3::cross( const vec3 & other ) const {
    return cross( *this, other );
}

double vec3::dot( const vec3 & v1 , const vec3 & v2 ) {
    return ( v1.x * v2.x + v1.y * v2.y + v1.z * v2.z );
}

vec3 vec3::cross( const vec3 & v1, const vec3 & v2 ) {
    return vec3( v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x );
}

vec3 operator*( double a, const vec3 & v ) {
    return v * a; 
}

std::ostream & operator<<( std::ostream & os, vec3 vec ) {
    os << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]";
    return os;
}

