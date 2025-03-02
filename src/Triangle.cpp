#include "Triangle.h"

Triangle::Triangle( const vec3 & a, const vec3 & b, const vec3 & c,
                   const vec3 & an, const vec3 & bn, const vec3 & cn )
    : m_A( a ), m_B( b ), m_C( c ),
      m_An( an ), m_Bn( bn ), m_Cn( cn ) {}

vec3 & Triangle::A() {
    return m_A;
}

vec3 & Triangle::B() {
    return m_B;
}

vec3 & Triangle::C() {
    return m_C;
}

std::ostream & operator<<( std::ostream & os, Triangle t ) {
    os << "vertices: " << t.m_A << ", " << t.m_B << ", " << t.m_C << "\n";
    os << "normals: " << t.m_An << ", " << t.m_Bn << ", " << t.m_Cn;
    return os;
}