#include "IntersectionData.h"

#include <limits>

IntersectionData::IntersectionData()
    : m_Hit( false ), m_T( std::numeric_limits<double>::infinity() ), m_HitTriangle( nullptr ), m_HitPoint( vec3() ) {}

IntersectionData::IntersectionData( double t, const Triangle * triangle, const vec3 & point )
    : m_Hit( true ), m_T( t ), m_HitTriangle( triangle ), m_HitPoint( point ) {}
