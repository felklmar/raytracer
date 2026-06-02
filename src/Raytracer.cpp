#include "Raytracer.h"
#include "Options.h"

#include <cmath>
#include <algorithm>

IntersectionData Raytracer::findNearestIntersection( const Ray & ray, const std::vector<Triangle> & triangles ) {
    IntersectionData nearestHit;

    for ( const Triangle & triangle : triangles ) {
        IntersectionData hit = ray.triangleIntersection( triangle ); 

        if ( hit.m_Hit && hit.m_T < nearestHit.m_T ) {
            nearestHit = hit;
        }
    }

    return nearestHit;
}

/*
bool Raytracer::isLightVisible( const Ray & ray, const std::vector<Triangle> & triangles, double distance ) {
    for ( const Triangle & triangle : triangles ) {
        // Perform intersection check using the IntersectionData
        IntersectionData hit = ray.triangleIntersection( triangle ); 

        // If an intersection is found within the distance to the light and not behind the ray's origin
        if ( hit.m_Hit && hit.m_T > 1e-9 && hit.m_T < distance ) {
            return false; // Light is blocked by this triangle
        }
    }
    return true;  // No intersections, light is visible
}
*/

bool Raytracer::isLightVisible( const Ray & ray, const Octree & octree, double distance ) {
    IntersectionData hit = octree.intersect( ray );
    return ( !hit.m_Hit || hit.m_T > distance - 1e-9 );
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

vec3 Raytracer::traceRay( size_t depth, const Ray & ray, const Octree & octree, const std::vector<std::shared_ptr<Triangle>> & lights ) {
    // Find nearest intersection with scene triangles
    IntersectionData intersection = octree.intersect( ray );

    if ( !intersection.m_Hit ) return vec3();  // No intersection, return black

    vec3 color;
    vec3 normal = intersection.m_HitTriangle->interpolateNormal( intersection.m_HitPoint ).normalize();

    // Loop over each light source in the scene
    for ( auto l : lights ) {

        auto lightEmission = l->getMaterial().getEmissive();
        auto lightArea = l->getArea();  

        for ( size_t i = 0; i < options.pointsPerLight; ++i ) {
            auto [lightPoint, lightNormal] = l->getRandomPoint();  

            vec3 vectorToLight = lightPoint - intersection.m_HitPoint; 
            vec3 dirToLight = vectorToLight.normalize();
            double distance = vectorToLight.length();

            // Create a small offset to avoid self-shadowing issues
            Ray shadowRay( intersection.m_HitPoint + normal * 1e-9, dirToLight );

            // Check if the light is visible at this intersection point
            if ( isLightVisible( shadowRay, octree, distance - 1e-4 ) ) {
                // Calculate the weight for the area light (assuming you are using a similar formula)
                auto dot = std::max( 0.0, lightNormal.dot( -dirToLight ) );  
                auto weight = lightArea * dot / ( static_cast<double>( options.pointsPerLight ) * ( distance * distance ) );  

                // Calculate the Phong shading model with the given light intensity
                color += calculatePhong( intersection.m_HitTriangle->getMaterial(), weight * lightEmission, dirToLight,
                                         normal, -ray.getDirection() );
            }
        }
    }

    vec3 colorR, colorT;
    if ( depth < options.maxRecursionDepth ) {
        
        vec3 v = ( -ray.getDirection() ).normalize();

        // reflected ray
        if ( intersection.m_HitTriangle->getMaterial().getSpecular().length() > 1e-9 ) {
            vec3 reflectedDir = ( 2 * normal * vec3::dot( normal, v ) - v ).normalize(); 
            //vec3 reflectedOrigin = intersection.m_HitPoint + normal * 1e-9;
            Ray reflectedRay(  intersection.m_HitPoint, reflectedDir );
            colorR = intersection.m_HitTriangle->getMaterial().getSpecular() * traceRay( depth + 1, reflectedRay, octree, lights );
        }

        // refracted ray 
        if ( intersection.m_HitTriangle->getMaterial().getTransimittance().length() > 1e-9 ) {     
            double iorSrc = 1.0;
            double iorDst = intersection.m_HitTriangle->getMaterial().getIOR();
            double dot = vec3::dot( v, normal );

            if ( dot < 0.0 ) {
                normal = -normal;
                std::swap( iorSrc, iorDst );
            }
            
            double eta = iorSrc / iorDst;
            double k = 1.0 - eta * eta * ( 1.0 - dot * dot );
            if ( k > 0.0 ) {
                vec3 refractedDir = ( - eta * v + ( eta * dot - std::sqrt( k ) ) * normal ).normalize();
                vec3 refractedOrigin = intersection.m_HitPoint + refractedDir * 1e-9;
                Ray refractedRay( refractedOrigin, refractedDir );
                colorT = intersection.m_HitTriangle->getMaterial().getTransimittance() * traceRay( depth + 1, refractedRay, octree, lights );
            }
        }
    }

    return color + colorR + colorT;
}
