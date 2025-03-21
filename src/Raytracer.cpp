#include "Raytracer.h"
#include "Options.h"

#include <cmath>
#include <algorithm>

std::pair<vec3, std::shared_ptr<Triangle>> Raytracer::findNearestIntersection( const Ray & ray, const std::vector<Triangle> & triangles ) {
    double t = std::numeric_limits<double>::infinity();
    std::shared_ptr<Triangle> trianglePtr; 

    for ( const Triangle & triangle : triangles ) {
        double tNew = ray.triangleIntersection( triangle );
        if ( tNew >= 0 && tNew < t ) {
            t = tNew;
            trianglePtr = std::make_shared<Triangle>( triangle );
        }
    }

    vec3 intersection = ( trianglePtr ) ? ray.getOrigin() + t * ray.getDirection() : vec3(); 
    return std::make_pair( intersection, trianglePtr );
}

bool Raytracer::isLightVisible( const Ray & ray, const std::vector<Triangle> & triangles, double distance ) {
    for ( const Triangle &triangle : triangles ) {
        // Perform intersection check
        double t = ray.triangleIntersection( triangle );
        
        // If an intersection is found within the distance to the light and not behind the ray's origin
        if ( t > 0 && t < distance ) {
            return false;
        }
    }
    return true;  // No intersections, light is visible
}

vec3 Raytracer::calculatePhong( const Material & material, const vec3 & lightIntensity,
                                const vec3 & dirToLight, const vec3 & normal, const vec3 & dirToCamera ) {
    // Normalize the normal to ensure correct calculations
    vec3 normalizedNormal = normal.normalize();

    // Ensure a minimum value for diffuse term when light is close to the same plane
    float diffuseFactor = std::max( 0.05, vec3::dot( dirToLight, normalizedNormal ) );
    vec3 diffuse = lightIntensity * material.getDiffuse() * diffuseFactor;

    // Correct light direction for reflection calculation
    vec3 lightDir = -dirToLight;
    vec3 reflectionDir = 2 * vec3::dot( normalizedNormal, lightDir ) * normalizedNormal - lightDir;

    // Ensure a non-negative specular factor and apply the shininess
    float specularFactor = std::pow( std::max( 0.0, vec3::dot( dirToCamera, reflectionDir ) ), material.getShininess() );
    vec3 specular = lightIntensity * material.getSpecular() * specularFactor;

    // Add the emissive value (no lighting calculation here, just the material's own emitted light)
    vec3 emissive = material.getEmissive();

    // Combine all components
    return diffuse + specular + emissive;
}

vec3 Raytracer::traceRay( size_t depth, const Ray & ray, const std::vector<Triangle> & triangles, const std::vector<std::shared_ptr<Triangle>> & lights ) {
    // Find nearest intersection with scene triangles
    auto intersection = findNearestIntersection( ray, triangles );

    if ( !intersection.second ) return vec3();  // No intersection, return black

    vec3 color;
    vec3 normal = intersection.second->interpolateNormal( intersection.first );

    // Loop over each light source in the scene
    for ( auto l : lights ) {

        auto lightEmission = l->getMaterial().getEmissive();
        auto lightArea = l->getArea();  

        for ( size_t i = 0; i < options.pointsPerLight; ++i ) {
            auto [lightPoint, lightNormal] = l->getRandomPoint();  

            vec3 vectorToLight = lightPoint - intersection.first; 
            vec3 dirToLight = vectorToLight.normalize();
            double distance = vectorToLight.length();

            // Create a small offset to avoid self-shadowing issues
            Ray shadowRay( intersection.first + normal * 1e-9, dirToLight );

            // Check if the light is visible at this intersection point
            if ( isLightVisible( shadowRay, triangles, distance - 1e-4 ) ) {
                // Calculate the weight for the area light (assuming you are using a similar formula)
                auto dot = std::max( 0.0, lightNormal.dot( -dirToLight ) );  
                auto weight = lightArea * dot / ( static_cast<double>( options.pointsPerLight ) * ( distance * distance ) );  

                // Calculate the Phong shading model with the given light intensity
                color += calculatePhong( intersection.second->getMaterial(), weight * lightEmission, dirToLight,
                                         normal, -ray.getDirection() );
            }
        }
    }

    vec3 colorR;
    if ( depth < options.maxRecursionDepth ) {
        vec3 v = ( -ray.getDirection() ).normalize();
        vec3 reflectedDir = ( 2 * normal * vec3::dot( normal, v ) - v ).normalize(); 
        Ray reflectedRay( intersection.first, reflectedDir );
        colorR = intersection.second->getMaterial().getSpecular() * traceRay( depth + 1, reflectedRay, triangles, lights );
        //vec3 colorT = traceRay();
    }

    return color + colorR;
}
