#include "Scene.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Ray.h"
#include "Raytracer.h"
#include "Options.h"

#include <fstream>
#include <sstream>
#include <memory>

Scene::Scene( const std::string & filename ) {
    std::ifstream file( filename );

    if ( !file.is_open() ) 
        throw std::runtime_error( "Cannot open file: " + filename );
    
    // read obj file name
    std::string line;
    std::getline( file, line );
    
    loadObj( "scenes/" + line );
    loadCamera( file );

    file.close();

    //int i = 0;
    //for ( auto t : m_Triangles )
    //    std::cout << i++ << "   " << t << std::endl;

    //std::cerr << "Building octree" << std::endl;
    m_Octree = std::make_unique<Octree>( m_Triangles, options.maxOctreeDepth, options.minAABBtriangles );
    //m_Octree->printOctree();
}

vec3 Scene::readVector( const std::string & line ) {
    std::stringstream ss( line );
    std::string propertyName;
    double x, y, z;
    ss >> propertyName >> x >> y >> z;
    return vec3( x, y, z );
}

void Scene::loadCamera( std::ifstream & filestream ) {
    std::string line;
    vec3 pos, up, dir;
    double fov; 

    // read pos
    std::getline( filestream, line );
    pos = readVector( line );

    // read up
    std::getline( filestream, line );
    up = readVector( line );

    // read dir
    std::getline( filestream, line );
    dir = readVector( line );

    // read fov
    std::getline( filestream, line );
    std::stringstream ss( line );
    std::string propertyName;
    ss >> propertyName >> fov;

    m_Camera = Camera( pos, up, dir, fov );
}

void Scene::loadObj( const std::string & filename ) {
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
    const auto & materials = objReader.GetMaterials();

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

            auto matIdx = static_cast<size_t>( shape.mesh.material_ids[i] );
            const auto & m = materials[matIdx];

            Material material(
                vec3( m.diffuse[0], m.diffuse[1], m.diffuse[2] ),
                vec3( m.specular[0], m.specular[1], m.specular[2] ),
                vec3( m.ambient[0], m.ambient[1], m.ambient[2] ),
                vec3( m.emission[0], m.emission[1], m.emission[2] ),
                vec3( m.transmittance[0], m.transmittance[1], m.transmittance[2] ),
                m.shininess, m.ior 
            );

            Triangle triangle( a, b, c, an, bn, cn, material );
            m_Triangles.push_back( triangle );
            
            if ( material.getEmissive().length() > 0 ) {
                m_Lights.push_back( std::make_shared<Triangle>( triangle ) );
            }
        }
    }
}

std::vector<vec3> Scene::render( size_t width, size_t height ) {
    vec3 b = vec3::cross( m_Camera.getDirection(), m_Camera.getUp() ).normalize();

    double g_w = 2 * tan( 0.5 * m_Camera.getFOV() );
    double g_h = g_w * ( height / width );

    vec3 q_w = g_w / ( width - 1 ) * b;
    vec3 q_h = g_h / ( height - 1 ) * m_Camera.getUp();

    vec3 p_00 = m_Camera.getDirection() - 0.5 * g_w * b + 0.5 * g_h * m_Camera.getUp();

    std::vector<vec3> colors;
    std::cerr << "processed rows: ";
    for ( size_t i = 0; i < height; i++ ) {
        for ( size_t j = 0; j < width; j++ ) {
            vec3 p_xy = p_00 + j * q_w - i * q_h;
            vec3 r_xy = p_xy.normalize();

            Ray primaryRay( m_Camera.getPosition(), r_xy );

            vec3 color = Raytracer::traceRay( 0, primaryRay, *m_Octree, m_Lights );
            colors.push_back( color );
        }    
        std::cerr << i << " ";
    }

    return colors;
}