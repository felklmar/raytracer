#include "Camera.h"

Camera::Camera( const vec3 & position, const vec3 & up, const vec3 & direction, double fov )
    : m_Position( position ), m_Up( up ), m_Direction( direction ), m_FOV( fov ) {}

vec3 Camera::getPosition() {
    return m_Position;
}

vec3 Camera::getUp() {
    return m_Up;
}

vec3 Camera::getDirection() {
    return m_Direction;
}

double Camera::getFOV() {
    return m_FOV;
}

std::ostream & operator<<( std::ostream & os, Camera c ) {
    os << "pos: " << c.m_Position << "\n"; 
    os << "up: " << c.m_Up << "\n";
    os << "dir: " << c.m_Direction << "\n";
    os << "fov: " << c.m_FOV;
    return os;
}