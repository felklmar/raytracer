#include "Material.h"

Material::Material( const vec3 & diffuse, const vec3 & specular, const vec3 & ambient,
                    const vec3 & emissive, const vec3 & transmittance, double shininess, double ior )
    : m_Diffuse( diffuse ), m_Specular( specular ), m_Ambient( ambient ), m_Emissive( emissive ), 
      m_Transimittance( transmittance ), m_Shininess( shininess ), m_IOR( ior ) {}

const vec3 & Material::getDiffuse() const {
    return m_Diffuse;
}

const vec3 & Material::getSpecular() const {
    return m_Specular;
}

const vec3 & Material::getEmissive() const {
    return m_Emissive;
}

const vec3 & Material::getTransimittance() const {
    return m_Transimittance;
}

double Material::getShininess() const {
    return m_Shininess;
}

double Material::getIOR() const {
    return m_IOR;
}

std::ostream & operator<<( std::ostream & os, Material m ) {
    os << "Diffuse: "   << m.m_Diffuse   << "\n";
    os << "Specular: "  << m.m_Specular  << "\n";
    os << "Ambient: "   << m.m_Ambient   << "\n";
    os << "Emissive: "  << m.m_Emissive  << "\n";
    os << "Shininess: " << m.m_Shininess << "\n";
    os << "IOR: "       << m.m_IOR;
    return os;
}