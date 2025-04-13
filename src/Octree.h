#pragma once

#include "BoundingBox.h"
#include "Triangle.h"

#include <vector>
#include <memory>
#include <iomanip>

struct OctreeNode {
    BoundingBox m_Bounds;
    std::unique_ptr<OctreeNode> m_Children[8];
    std::vector<size_t> m_TriangleIndices;

    OctreeNode(); 
};

struct HitInfo {

};

class Octree {
    private:
        std::unique_ptr<OctreeNode> m_Root; 
        const std::vector<Triangle>* m_Triangles; 
        int m_MaxDepth; 

        void buildNode( OctreeNode* node, const std::vector<size_t> & indices, int depth ); 
        void subdivideNode( int depth, const BoundingBox & parentBounds, OctreeNode* parentNode );
        //void naiveIntersect( OctreeNode* node, const Ray & ray, HitInfo & hitInfo, float & closestT, bool & hit ) const; 
        BoundingBox calculateBounds( const std::vector<Triangle> & triangles ) const;
        
        void printOctreeNode(const OctreeNode* node, int depth) const {
            if (!node) return;
    
            // Indentation for better visualization
            std::cout << std::setw( depth * 4 ) << "" << "Depth " << depth << " - Bounds: " << node->m_Bounds
                      << ", Triangles: " << node->m_TriangleIndices.size() << std::endl;
    
            for (int i = 0; i < 8; ++i) {
                printOctreeNode( node->m_Children[i].get(), depth + 1 );
            }
        }
    public:
        Octree();
        Octree( const std::vector<Triangle> & triangles, int maxDepth = 2 ); 

        void build(); 
        //bool intersect( const Ray & ray, HitInfo & hitInfo ) const; 

        void printOctree() const {
            if ( m_Root ) {
                printOctreeNode( m_Root.get(), 0 );
            }
            else {
                std::cout << "Octree is empty." << std::endl;
            }
        }
};
