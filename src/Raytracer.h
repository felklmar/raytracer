#pragma once

#include "vec3.h"
#include "Ray.h"
#include "Triangle.h"

#include <vector>
#include <memory>

class Raytracer {
    private:
        static std::pair<vec3, std::shared_ptr<Triangle>> findNearestIntersection( const Ray & ray, const std::vector<Triangle> & triangles );
        static bool isLightVisible( const Ray & ray, const std::vector<Triangle> & triangles, double distance );
        static vec3 calculatePhong( const Material & material, const vec3 & lightIntensity,
                                    const vec3 & dirToLight, const vec3 & normal, const vec3 & dirToCamera );
        static vec3 calculateIlluminatedIntensity( const vec3 & P, const Triangle & triangle, const vec3 & lightIntensity );     
    public:
        static vec3 traceRay( size_t depth, const Ray & ray, const std::vector<Triangle> & triangles, 
                              const std::vector<std::shared_ptr<Triangle>> & lights );
};