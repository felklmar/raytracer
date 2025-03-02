#pragma once

#include "vec3.h"

class Triangle {
    private:
        vec3 m_A, m_B, m_C;
        vec3 m_An, m_Bn, m_Cn;
    public:
        Triangle( const vec3 & a, const vec3 & b, const vec3 & c,
                  const vec3 & an, const vec3 & bn, const vec3 & cn );

        vec3 & A();
        vec3 & B();
        vec3 & C();

        friend std::ostream & operator<<( std::ostream & os, Triangle t );
};
