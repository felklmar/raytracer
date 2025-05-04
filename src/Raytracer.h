#pragma once

#include "vec3.h"
#include "Ray.h"
#include "IntersectionData.h"
#include "Octree.h"

#include <vector>
#include <memory>

class Raytracer {
    private:
        static IntersectionData findNearestIntersection( const Ray & ray, const std::vector<Triangle> & triangles );
        //static bool isLightVisible( const Ray & ray, const std::vector<Triangle> & triangles, double distance );
        static bool isLightVisible( const Ray & ray, const Octree & octree, double distance );
        static vec3 calculatePhong( const Material & material, const vec3 & lightIntensity,
                                    const vec3 & dirToLight, const vec3 & normal, const vec3 & dirToCamera );
    public:
        static vec3 traceRay( size_t depth, const Ray & ray, const Octree & octree, 
                              const std::vector<std::shared_ptr<Triangle>> & lights );
};