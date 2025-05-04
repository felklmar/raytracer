#pragma once

#include "Triangle.h"

struct IntersectionData {
    bool m_Hit;
    double m_T;
    const Triangle * m_HitTriangle;
    vec3 m_HitPoint;

    IntersectionData();
    IntersectionData( double t, const Triangle * triangle, const vec3 & point );
};