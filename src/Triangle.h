#pragma once

#include "vec3.h"
#include "Material.h"
#include "BoundingBox.h"

class Triangle {
    private:
        vec3 m_A, m_B, m_C;
        vec3 m_An, m_Bn, m_Cn;
        Material m_Material;
    public:
        Triangle( const vec3 & a, const vec3 & b, const vec3 & c,
                  const vec3 & an, const vec3 & bn, const vec3 & cn,
                  const Material & material );

        const vec3 & getA() const;
        const vec3 & getB() const;
        const vec3 & getC() const;
        const Material & getMaterial() const;

        vec3 interpolateNormal( const vec3 & point ) const;
        double getArea() const;
        std::pair<vec3, vec3> getRandomPoint() const;

        BoundingBox getBounds() const;
        friend std::ostream & operator<<( std::ostream & os, Triangle t );
};
