#include "Camera.h"

#include <fstream>
#include <sstream>

Camera::Camera( const std::string & filename ) {
    std::ifstream file( filename );

    if ( !file.is_open() ) 
        throw std::runtime_error( "Cannot open file: " + filename );
    
    std::string line;
    
    // read pos
    std::getline( file, line );
    std::getline( file, line );
    m_Position = readVector( line );

    // read up
    std::getline( file, line );
    m_Up = readVector( line );

    // read dir
    std::getline( file, line );
    m_Direction = readVector( line );

    // read fov
    std::getline( file, line );
    std::stringstream ss( line );
    std::string propertyName;
    ss >> propertyName >> m_FOV;

    file.close();
}

vec3 Camera::readVector( const std::string & line ) {
    std::stringstream ss( line );
    std::string propertyName;
    double x, y, z;
    ss >> propertyName >> x >> y >> z;
    return vec3( x, y, z );
}

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