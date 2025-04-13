#include "Triangle.h"
#include <random>

Triangle::Triangle( const vec3 & a, const vec3 & b, const vec3 & c,
                    const vec3 & an, const vec3 & bn, const vec3 & cn,
                    const Material & material )
    : m_A( a ), m_B( b ), m_C( c ),
      m_An( an ), m_Bn( bn ), m_Cn( cn ),
      m_Material( material ) {}

const vec3 & Triangle::getA() const {
    return m_A;
}

const vec3 & Triangle::getB() const {
    return m_B;
}

const vec3 & Triangle::getC() const {
    return m_C;
}

const Material & Triangle::getMaterial() const {
    return m_Material;
}

vec3 Triangle::interpolateNormal( const vec3 & point ) const {
    // Compute vectors
    vec3 v0 = m_B - m_A, v1 = m_C - m_A, v2 = point - m_A;

    // Compute dot products
    float d00 = vec3::dot( v0, v0 );
    float d01 = vec3::dot( v0, v1 );
    float d11 = vec3::dot( v1, v1 );
    float d20 = vec3::dot( v2, v0 );
    float d21 = vec3::dot( v2, v1 );

    // Compute determinant
    float denom = d00 * d11 - d01 * d01;

    // Compute barycentric coordinates
    float v = ( d11 * d20 - d01 * d21 ) / denom;
    float w = ( d00 * d21 - d01 * d20 ) / denom;
    float u = 1.0f - v - w; 

    // Interpolate normal
    vec3 interpolatedNormal = u * m_An + v * m_Bn + w * m_Cn;
    return interpolatedNormal.normalize();
}

double Triangle::getArea() const {
    // Cross product of two sides of the triangle gives the area of the parallelogram
    // Area of the triangle is half the area of the parallelogram
    vec3 edge1 = m_B - m_A;
    vec3 edge2 = m_C - m_A;
    return 0.5 * edge1.cross( edge2 ).length();
}

std::pair<vec3, vec3> Triangle::getRandomPoint() const {
    static std::default_random_engine rng;
    static std::uniform_real_distribution<double> dist(0.0, 1.0);

    // Generate random numbers
    double r1 = dist( rng );
    double r2 = dist( rng );

    // Barycentric coordinates
    double u = 1 - std::sqrt( r1 );
    double v = (1 - r2) * std::sqrt( r1 );

    // Return the point on the triangle using barycentric coordinates
    vec3 point = m_A + (m_B - m_A) * u + (m_C - m_A) * v;
    
    // Interpolate the normal at the point using barycentric coordinates
    auto normal = interpolateNormal( point );

    return { point, normal };
}

BoundingBox Triangle::getBounds() const {
    BoundingBox bounds;
    bounds.expand( m_A );
    bounds.expand( m_B );
    bounds.expand( m_C );
    return bounds;
}

std::ostream & operator<<( std::ostream & os, Triangle t ) {
    os << "vertices: " << t.m_A << ", " << t.m_B << ", " << t.m_C << "\n";
    os << "normals: " << t.m_An << ", " << t.m_Bn << ", " << t.m_Cn;
    return os;
}