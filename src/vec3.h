#pragma once

#include <iostream>

const double epsilon = 1e-9; // Small value

struct vec3 {
    double x, y, z;

    vec3();
    vec3( double v );
    vec3( double x0, double y0, double z0 = 0 );
    vec3 operator*( double a ) const;
    vec3 operator*( const vec3 r ) const;
    vec3 operator/( const double r ) const;
    vec3 operator+( const vec3 & v ) const;
    vec3 operator-( const vec3 & v ) const;
    vec3 operator-() const;
    void operator+=( const vec3 & v );
    void operator*=( double a );
    void operator*=( const vec3 & v );
    double length() const;
    double average();
    vec3 normalize() const;

    double dot( const vec3 & other ) const;
    vec3 cross( const vec3 & other ) const;

    static double dot( const vec3 & v1, const vec3 & v2 );
    static vec3 cross( const vec3 & v1, const vec3 & v2 );

    friend vec3 operator*( double a, const vec3 & v );
    friend std::ostream & operator<<( std::ostream & os, vec3 vec );
};
