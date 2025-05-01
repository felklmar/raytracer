#pragma once

#include "vec3.h"
#include "Ray.h"
#include "Triangle.h"

class BoundingBox {
    private:
        vec3 m_Min; 
        vec3 m_Max; 

        std::pair<double,double> project( const std::vector<vec3> & points, const vec3 & axis ) const;
        std::vector<vec3> getVertices() const;
    public:
        BoundingBox();
        BoundingBox( const vec3 & min, const vec3 & max );
        BoundingBox( const std::pair<vec3, vec3> & minmax );
    
        bool isValid() const;
        BoundingBox & expand( const vec3 & point ); 
        void expand( const BoundingBox & other ); 
    
        vec3 center() const;
        vec3 dimensions() const;

        vec3 getMin() const;
        vec3 getMax() const;

        //bool intersect( const Ray & ray, float & tNear, float & tFar ) const;

        bool intersects( const BoundingBox & other ) const;
        bool intersects( const Triangle & t ) const;

        friend std::ostream & operator<<( std::ostream & out, const BoundingBox & box );
};