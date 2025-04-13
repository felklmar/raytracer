#include "Octree.h"

#include <limits>

OctreeNode::OctreeNode() 
    : m_Bounds(), m_TriangleIndices() {
        for ( int i = 0; i < 8; ++i ) m_Children[i] = nullptr;
    }

Octree::Octree()
    : m_Root( nullptr ), m_Triangles( nullptr ), m_MaxDepth( 10 ) {}

Octree::Octree( const std::vector<Triangle> & triangles, int maxDepth )
    : m_Root( std::make_unique<OctreeNode>() ),
      m_Triangles( &triangles ),
      m_MaxDepth( maxDepth ) {
    build();
}

void Octree::build() {
    if ( m_Triangles == nullptr || m_Triangles->empty() ) return;

    BoundingBox sceneBounds = calculateBounds( *m_Triangles );
    m_Root->m_Bounds = sceneBounds;

    std::vector<size_t> allIndices( m_Triangles->size() );
    for ( size_t i = 0; i < m_Triangles->size(); ++i )
        allIndices[i] = i;

    buildNode( m_Root.get(), allIndices, 0 );
}

void Octree::buildNode( OctreeNode* node, const std::vector<size_t> & indices, int depth ) {
    node->m_TriangleIndices = indices;

    if ( depth >= m_MaxDepth ) return;

    subdivideNode( depth, node->m_Bounds, node );

    std::vector<size_t> childIndices[8]; // Array of index vectors for children

    // Distribute triangles to children
    for ( size_t triangleIndex : indices ) {
        const Triangle & triangle = (*m_Triangles)[triangleIndex]; // Get the actual triangle
        BoundingBox triBounds = triangle.getBounds();

        for ( int i = 0; i < 8; ++i ) {
            if ( node->m_Children[i]->m_Bounds.intersect( triBounds ) ) {
                childIndices[i].push_back( triangleIndex );
            }
        }
    }

    // Recursive calls with appropriate indices
    for ( int i = 0; i < 8; ++i ) {
        if ( node->m_Children[i] == nullptr )
             node->m_Children[i] = std::make_unique<OctreeNode>();
        buildNode( node->m_Children[i].get(), childIndices[i], depth + 1 );
    }
}

void Octree::subdivideNode( int depth, const BoundingBox & parentBounds, OctreeNode* parentNode ) {
    vec3 center = parentBounds.center();
    vec3 dimensions = parentBounds.dimensions();
    vec3 halfDim = dimensions / 2.0f;

    parentNode->m_Children[0] = std::make_unique<OctreeNode>();
    parentNode->m_Children[1] = std::make_unique<OctreeNode>();
    parentNode->m_Children[2] = std::make_unique<OctreeNode>();
    parentNode->m_Children[3] = std::make_unique<OctreeNode>();
    parentNode->m_Children[4] = std::make_unique<OctreeNode>();
    parentNode->m_Children[5] = std::make_unique<OctreeNode>();
    parentNode->m_Children[6] = std::make_unique<OctreeNode>();
    parentNode->m_Children[7] = std::make_unique<OctreeNode>();

    parentNode->m_Children[0]->m_Bounds = BoundingBox( parentBounds.getMin(), center );
    parentNode->m_Children[1]->m_Bounds = BoundingBox( vec3( parentBounds.getMin().x, parentBounds.getMin().y, center.z ), vec3( center.x, center.y, parentBounds.getMax().z ) );
    parentNode->m_Children[2]->m_Bounds = BoundingBox( vec3( parentBounds.getMin().x, center.y, parentBounds.getMin().z ), vec3( center.x, parentBounds.getMax().y, center.z ) );
    parentNode->m_Children[3]->m_Bounds = BoundingBox( vec3( parentBounds.getMin().x, center.y, center.z ), vec3( center.x, parentBounds.getMax().y, parentBounds.getMax().z ) );
    parentNode->m_Children[4]->m_Bounds = BoundingBox( vec3( center.x, parentBounds.getMin().y, parentBounds.getMin().z ), vec3( parentBounds.getMax().x, center.y, center.z ) );
    parentNode->m_Children[5]->m_Bounds = BoundingBox( vec3( center.x, parentBounds.getMin().y, center.z ), vec3( parentBounds.getMax().x, center.y, parentBounds.getMax().z ) );
    parentNode->m_Children[6]->m_Bounds = BoundingBox( vec3( center.x, center.y, parentBounds.getMin().z ), vec3( parentBounds.getMax().x, parentBounds.getMax().y, center.z ) );
    parentNode->m_Children[7]->m_Bounds = BoundingBox( center, parentBounds.getMax() );
}

BoundingBox Octree::calculateBounds( const std::vector<Triangle> & triangleData ) const {
    BoundingBox bounds;
    for ( const Triangle & triangle : triangleData ) {
        bounds.expand( triangle.getBounds() );
    }
    return bounds;
}