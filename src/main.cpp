#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <cfloat>
#include <algorithm>

#include "vec3.h"
#include "Ray.h"
#include "Camera.h"
#include "Triangle.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define WIDTH 800
#define HEIGHT 800

template <typename T>
inline T clamp( T val, T low, T high ) {
    return max( min( val, high ), low );
}

/*struct Sphere { 
    Sphere(float rad_, vec3 p_): 
        rad(rad_), p(p_) {} 
   
    float intersect(const Ray &r) const { // returns distance, 0 if nohit 
        vec3 op = p - r.o; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0 
        float t, eps=1e-4, b=op.dot(r.d), det=b*b-op.dot(op)+rad*rad; 
        if (det<0) return 0; else det=sqrt(det); 
        return (t=b-det)>eps ? t : ((t=b+det)>eps ? t : 0); 
    } 

    float rad;  // radius 
    vec3 p;     // position
}; */

std::vector<Triangle> loadObj( const std::string & filename ) {
    
    tinyobj::ObjReader objReader {};
    {
        objReader.ParseFromFile( filename );
        
        std::cerr << objReader.Warning();
        std::cerr << objReader.Error();
        if( !objReader.Valid() )
            throw "Failed to load OBJ file";
    }
    
    const auto & vertices = objReader.GetAttrib().vertices;
    const auto & normals = objReader.GetAttrib().normals;
    
    std::vector<Triangle> triangles;
    for ( const auto & shape : objReader.GetShapes() ) {
        std::size_t trianglesNum = shape.mesh.indices.size() / 3;
        const auto & indices = shape.mesh.indices;

        for( size_t i = 0; i < trianglesNum; i++ ) {

            auto aIdx = static_cast<std::size_t>( indices[3 * i + 0].vertex_index );
            auto bIdx = static_cast<std::size_t>( indices[3 * i + 1].vertex_index );
            auto cIdx = static_cast<std::size_t>( indices[3 * i + 2].vertex_index );
            
            vec3 a( vertices[3 * aIdx + 0], vertices[3 * aIdx + 1], vertices[3 * aIdx + 2] );
            vec3 b( vertices[3 * bIdx + 0], vertices[3 * bIdx + 1], vertices[3 * bIdx + 2] );
            vec3 c( vertices[3 * cIdx + 0], vertices[3 * cIdx + 1], vertices[3 * cIdx + 2] );

            auto anIdx = static_cast<std::size_t>( indices[3 * i + 0].normal_index );
            auto bnIdx = static_cast<std::size_t>( indices[3 * i + 1].normal_index );
            auto cnIdx = static_cast<std::size_t>( indices[3 * i + 2].normal_index );

            vec3 an, bn, cn;
            if ( normals.empty() ) {
                auto normal = ( b - a ).cross( c - a ).normalize();
                an = bn = cn = normal;
            } else {
                an = {normals[3 * anIdx + 0], normals[3 * anIdx + 1], normals[3 * anIdx + 2]};
                bn = {normals[3 * bnIdx + 0], normals[3 * bnIdx + 1], normals[3 * bnIdx + 2]};
                cn = {normals[3 * cnIdx + 0], normals[3 * cnIdx + 1], normals[3 * cnIdx + 2]};
            }

            triangles.emplace_back( a, b, c, an, bn, cn );
        }
    }
    
    return triangles;
}

int main( int argc, char const *argv[] ) {

    Camera camera( "scenes/CornellBox-Original.txt" );    
    //std::cout << camera << std::endl;
    
    std::vector<Triangle> triangles = loadObj( "scenes/CornellBox-Original.obj" );

    //for ( const Triangle & t : triangles )
    //    std::cout << t << "\n" << std::endl;

    vec3 b = vec3::cross( camera.getDirection(), camera.getUp() ).normalize();

    double g_w = 2 * tan( 0.5 * camera.getFOV() );
    double g_h = g_w * ( HEIGHT / WIDTH );

    vec3 q_w = g_w / ( WIDTH - 1 ) * b;
    vec3 q_h = g_h / ( HEIGHT - 1 ) * camera.getUp();

    vec3 p_00 = camera.getDirection() - 0.5 * g_w * b + 0.5 * g_h * camera.getUp();

    std::vector<double> depths;
    std::vector<vec3> intersections;
    //size_t rayCount = 0;
    for ( size_t i = 0; i < HEIGHT; i++ ) {
        for ( size_t j = 0; j < WIDTH; j++ ) {
            vec3 p_xy = p_00 + j * q_w - i * q_h;
            vec3 r_xy = p_xy.normalize();

            Ray primaryRay( camera.getPosition(), r_xy );
            
            double t = std::numeric_limits<double>::infinity();
            for ( Triangle & triangle : triangles ) {
                t = std::min( t, primaryRay.triangleIntersection( triangle ) );
            }

            depths.push_back( t );
            intersections.emplace_back( primaryRay.o() + primaryRay.s() * t );

            //std::cout << primaryRay << std::endl;
            //rayCount++;
        }    
    }

    //std::cout << depths.size() << std::endl;
    //double t_min = *std::min_element( depths.begin(), depths.end() );
    //double t_max = *std::max_element( depths.begin(), depths.end() );
    //std::cout << t_min << " " << t_max << std::endl;

    double first = DBL_MIN, second = DBL_MIN;
    for ( size_t i = 0; i < depths.size(); i++ ) {
        if ( depths[i] > first ) {
            second = first;
            first = depths[i];
        } else if ( depths[i] > second && depths[i] < first) {
            second = depths[i];
        }
    }

    //std::cout << first << " " << second << std::endl;
    double t_min = *std::min_element( depths.begin(), depths.end() );
    double t_max = 1.25 * second;
    std::cout << t_min << " " << t_max << std::endl;

    //std::cout << "Počet paprsků: " << rayCount << std::endl;

    float * image = new float[WIDTH * HEIGHT * 3];

    /*
    //creating checker texture
    for( int i = 0; i < WIDTH; ++i )
        for( int j = 0; j < HEIGHT; ++j )
            for( int k = 0; k < 3; ++k )
                image[3 * (j * WIDTH + i) + k] = (i + j) % 2 * 255;
    */
   
    for( int i = 0; i < WIDTH; ++i )
        for( int j = 0; j < HEIGHT; ++j )
            for( int k = 0; k < 3; ++k )
                image[3 * (j * WIDTH + i) + k] = ( std::isinf( depths[j * WIDTH + i] ) ) ? 0.0 : 255.0 + ( depths[j * WIDTH + i] - t_min ) * ( 0.0 - 255.0 ) / ( t_max - t_min );

    std::ofstream out( "t.txt" );
    for ( size_t i = 0; i < depths.size(); i++ ) 
        out << depths[i] << " " << intersections[i] << std::endl;

    //image saving
    std::ofstream output( "output.ppm" );   
    output << "P3\n" << WIDTH << " " << HEIGHT << "\n" << 255 << std::endl;
    for ( int i = 0; i < WIDTH * HEIGHT * 3; ++i ) 
        output << (int) image[i] << " ";
    output.close();
}
