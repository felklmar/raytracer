#pragma once

#include "vec3.h"

class Material {
    private:
        vec3 m_Diffuse;
        vec3 m_Specular;
        vec3 m_Ambient;
        vec3 m_Emissive;
        double m_Shininess;
        double m_IOR;
    public:
        Material() = default;
        Material( const vec3 & diffuse, const vec3 & specular, const vec3 & ambient,
                  const vec3 & emissive, double shininess, double ior );

        const vec3 & getDiffuse() const;
        const vec3 & getSpecular() const;
        const vec3 & getEmissive() const;
        double getShininess() const;
        friend std::ostream & operator<<( std::ostream & os, Material m );
};