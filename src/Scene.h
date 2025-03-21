#pragma once

#include "Camera.h"
#include "Triangle.h"
#include "vec3.h"

#include <vector>
#include <memory>

class Scene {
    private:
        Camera m_Camera;
        std::vector<Triangle> m_Triangles;
        std::vector<std::shared_ptr<Triangle>> m_Lights;

        vec3 readVector( const std::string & line );
        
        void loadCamera( std::ifstream & filestream );
        void loadObj( const std::string & filename );
    public:
        Scene( const std::string & filename );
        std::vector<vec3> render( size_t width, size_t height );      
};