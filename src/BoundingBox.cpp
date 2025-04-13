#include "BoundingBox.h"
#include <limits>

BoundingBox::BoundingBox() 
    : m_Min( std::numeric_limits<float>::infinity() ), 
      m_Max( -std::numeric_limits<float>::infinity() ) {} 

BoundingBox::BoundingBox( const vec3 & min, const vec3 & max )
    : m_Min( min ), m_Max( max ) {}

bool BoundingBox::isValid() const { 
    return m_Min.x <= m_Max.x && m_Min.y <= m_Max.y && m_Min.z <= m_Max.z; 
}

void BoundingBox::expand( const vec3 & point ) {
    m_Min.x = std::min( m_Min.x, point.x );
    m_Min.y = std::min( m_Min.y, point.y );
    m_Min.z = std::min( m_Min.z, point.z );

    m_Max.x = std::max( m_Max.x, point.x );
    m_Max.y = std::max( m_Max.y, point.y );
    m_Max.z = std::max( m_Max.z, point.z );
}

void BoundingBox::expand( const BoundingBox & other ) {
    expand( other.m_Min );
    expand( other.m_Max );
}

vec3 BoundingBox::center() const { 
    return ( m_Min + m_Max ) * 0.5f; 
}

vec3 BoundingBox::dimensions() const { 
    return m_Max - m_Min; 
}

vec3 BoundingBox::getMin() const {
    return m_Min;
}

vec3 BoundingBox::getMax() const {
    return m_Max;
}

bool BoundingBox::intersect( const BoundingBox & other ) const {
    return ( m_Min.x <= other.m_Max.x && m_Max.x >= other.m_Min.x ) &&
           ( m_Min.y <= other.m_Max.y && m_Max.y >= other.m_Min.y ) &&
           ( m_Min.z <= other.m_Max.z && m_Max.z >= other.m_Min.z );
}