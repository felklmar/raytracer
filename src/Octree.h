#pragma once

#include "BoundingBox.h"
#include "IntersectionData.h"

#include <vector>
#include <memory>
#include <iomanip>

struct OctreeNode {
    int m_Id;
    BoundingBox m_Bounds;
    std::unique_ptr<OctreeNode> m_Children[8];
    std::vector<size_t> m_TriangleIndices;

    OctreeNode(); 
    OctreeNode( int id ); 
};

class Octree {
    private:
        std::unique_ptr<OctreeNode> m_Root; 
        const std::vector<Triangle>* m_Triangles; 
        int m_MaxDepth; 
        size_t m_MinTrianglesInCell;

        void buildNode( OctreeNode* node, const std::vector<size_t> & indices, int depth ); 
        void subdivideNode( OctreeNode* parentNode );
        BoundingBox calculateBounds( const std::vector<Triangle> & triangles ) const;
        void intersectRayNode( const OctreeNode * node, const Ray & ray, IntersectionData & closestIntersection ) const;
        bool rayAABBIntersect( const BoundingBox & bounds, const Ray & ray, double & tNear, double & tFar ) const;
        void getTraversalOrder( const BoundingBox & bounds,  const Ray & ray, int outOrder[8] ) const;

        void printOctreeNode(const OctreeNode* node, int depth) const {
            if (!node) return;
    
            // Indentation for better visualization
            std::cout << std::setw( depth * 4 ) << "" << "Depth " << depth 
                      << " - Id: " << node->m_Id
                      << ", Bounds: " << node->m_Bounds
                      << ", Triangles: " << node->m_TriangleIndices.size() << " - [ ";
            
            for ( auto ti : node->m_TriangleIndices )
                std::cout << ti << " ";

            std::cout << "]" << std::endl;

            for (int i = 0; i < 8; ++i) {
                printOctreeNode( node->m_Children[i].get(), depth + 1 );
            }
        }
    public:
        Octree();
        Octree( const std::vector<Triangle> & triangles, int maxDepth = 3, int minTrianglesInCell = 5 ); 

        void build(); 
        IntersectionData intersect( const Ray & ray ) const;

        void printOctree() const {
            if ( m_Root ) {
                printOctreeNode( m_Root.get(), 0 );
            }
            else {
                std::cout << "Octree is empty." << std::endl;
            }
        }
};
