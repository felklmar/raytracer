#pragma once

#include "vec3.h"
#include "Ray.h"

class BoundingBox {
    private:
        vec3 m_Min; 
        vec3 m_Max; 
    public:
        BoundingBox();
        BoundingBox( const vec3 & min, const vec3 & max );
    
        bool isValid() const;
        void expand( const vec3 & point ); 
        void expand( const BoundingBox & other ); 
    
        vec3 center() const;
        vec3 dimensions() const;

        vec3 getMin() const;
        vec3 getMax() const;

        //bool intersect( const Ray & ray, float & tNear, float & tFar ) const;

        bool intersect( const BoundingBox & other ) const;

        friend std::ostream& operator<<( std::ostream & out, const BoundingBox & box ) {
            out << "Min: " << box.m_Min << ", Max: " << box.m_Max;
            return out;
        }
};