#include "Octree.h"

#include <limits>
#include <algorithm>

OctreeNode::OctreeNode() 
    : m_Id( 0 ), m_Bounds(), m_TriangleIndices() {
    for ( int i = 0; i < 8; ++i ) m_Children[i] = nullptr;
}

OctreeNode::OctreeNode( int id ) 
    : m_Id( id ), m_Bounds(), m_TriangleIndices() {
    for ( int i = 0; i < 8; ++i ) m_Children[i] = nullptr;
}

Octree::Octree()
    : m_Root( nullptr ), m_Triangles( nullptr ), m_MaxDepth( 3 ), m_MinTrianglesInCell( 5 ) {}

Octree::Octree( const std::vector<Triangle> & triangles, int maxDepth, int minTrianglesInCell )
    : m_Root( std::make_unique<OctreeNode>() ),
      m_Triangles( &triangles ),
      m_MaxDepth( maxDepth ),
      m_MinTrianglesInCell( minTrianglesInCell ) {
    build();
}

void Octree::build() {
    if ( m_Triangles == nullptr || m_Triangles->empty() ) return;

    m_Root->m_Bounds = calculateBounds( *m_Triangles );
    std::vector<size_t> allIndices( m_Triangles->size() );

    for ( size_t i = 0; i < m_Triangles->size(); ++i )
        allIndices[i] = i;

    buildNode( m_Root.get(), allIndices, 0 );
}

void Octree::buildNode( OctreeNode* node, const std::vector<size_t> & indices, int depth ) {
    node->m_TriangleIndices = indices;

    // Check if the number of triangles is below the threshold
    if ( indices.size() <= m_MinTrianglesInCell || depth >= m_MaxDepth ) return; 

    subdivideNode( node );
    std::vector<size_t> childIndices[8]; 

    // Distribute triangles to children
    for ( size_t triangleIndex : indices ) {
        const Triangle & triangle = (*m_Triangles)[triangleIndex]; 

        for ( int i = 0; i < 8; ++i ) {
            if ( node->m_Children[i]->m_Bounds.intersects( triangle ) ) {
                childIndices[i].push_back( triangleIndex );
            }
        }
    }

    for ( int i = 0; i < 8; ++i ) {
        if ( !childIndices[i].empty() ) { 
            if ( node->m_Children[i] == nullptr ) {
                node->m_Children[i] = std::make_unique<OctreeNode>();
            }
            buildNode( node->m_Children[i].get(), childIndices[i], depth + 1 );
        } else {
            node->m_Children[i] = nullptr;
        }
    }
}

void Octree::subdivideNode( OctreeNode* parentNode ) {
    const BoundingBox & parentBounds = parentNode->m_Bounds;
    vec3 center = parentBounds.center();
    vec3 min = parentBounds.getMin();
    vec3 max = parentBounds.getMax();

    for ( int i = 0; i < 8; ++i )
        parentNode->m_Children[i] = std::make_unique<OctreeNode>( i );

    parentNode->m_Children[0]->m_Bounds = BoundingBox( min, center );
    parentNode->m_Children[1]->m_Bounds = BoundingBox( vec3( center.x, min.y, min.z ), vec3( max.x, center.y, center.z ) );
    parentNode->m_Children[2]->m_Bounds = BoundingBox( vec3( min.x, center.y, min.z ), vec3( center.x, max.y, center.z ) );
    parentNode->m_Children[3]->m_Bounds = BoundingBox( vec3( center.x, center.y, min.z ), vec3( max.x, max.y, center.z ) );

    parentNode->m_Children[4]->m_Bounds = BoundingBox( vec3( min.x, min.y, center.z ), vec3( center.x, center.y, max.z ) );
    parentNode->m_Children[5]->m_Bounds = BoundingBox( vec3( center.x, min.y, center.z ), vec3( max.x, center.y, max.z ) );
    parentNode->m_Children[6]->m_Bounds = BoundingBox( vec3( min.x, center.y, center.z ), vec3( center.x, max.y, max.z ) );
    parentNode->m_Children[7]->m_Bounds = BoundingBox( center, max );
}

BoundingBox Octree::calculateBounds( const std::vector<Triangle> & triangleData ) const {
    BoundingBox bounds;
    for ( const Triangle & triangle : triangleData )
        bounds.expand( triangle.getBounds() );
    return bounds;
}

bool Octree::rayAABBIntersect( const BoundingBox & bounds, const Ray & ray, double & tNear, double & tFar ) const {
    tNear = -std::numeric_limits<double>::infinity();
    tFar = std::numeric_limits<double>::infinity();

    for ( int i = 0; i < 3; ++i ) {
        if ( std::abs( ray.getDirection()[i] ) < 1e-6 ) { // Ray is parallel to the slab
            if ( ray.getOrigin()[i] < bounds.getMin()[i] || ray.getOrigin()[i] > bounds.getMax()[i] ) {
                return false; // Ray is outside the slab, no intersection
            }
        } else {
            double t1 = ( bounds.getMin()[i] - ray.getOrigin()[i] ) / ray.getDirection()[i];
            double t2 = ( bounds.getMax()[i] - ray.getOrigin()[i] ) / ray.getDirection()[i];

            if ( t1 > t2 ) std::swap( t1, t2 );

            tNear = std::max( tNear, t1 );
            tFar = std::min( tFar, t2 );

            if ( tNear > tFar ) return false; // Intersection interval is empty
        }
    }

    return true; // Ray intersects the AABB
}

void Octree::getTraversalOrder( const BoundingBox & bounds,  const Ray & ray, int outOrder[8] ) const {
    vec3 center = bounds.center();
    bool right = ray.getDirection().x > 0;
    bool top = ray.getDirection().y > 0;
    bool front = ray.getDirection().z > 0;

    // Order based on the octant the ray is moving towards
    outOrder[0] = ( right ? 4 : 0 ) + ( top ? 2 : 0 ) + ( front ? 1 : 0 );
    outOrder[1] = ( right ? 4 : 0 ) + ( top ? 2 : 0 ) + ( front ? 0 : 1 );
    outOrder[2] = ( right ? 4 : 0 ) + ( top ? 0 : 2 ) + ( front ? 1 : 0 );
    outOrder[3] = ( right ? 4 : 0 ) + ( top ? 0 : 2 ) + ( front ? 0 : 1 );
    outOrder[4] = ( right ? 0 : 4 ) + ( top ? 2 : 0 ) + ( front ? 1 : 0 );
    outOrder[5] = ( right ? 0 : 4 ) + ( top ? 2 : 0 ) + ( front ? 0 : 1 );
    outOrder[6] = ( right ? 0 : 4 ) + ( top ? 0 : 2 ) + ( front ? 1 : 0 );
    outOrder[7] = ( right ? 0 : 4 ) + ( top ? 0 : 2 ) + ( front ? 0 : 1 );

    // Reorder based on entry point relative to the center
    int entryOrder[8];
    double t[8];

    for ( int i = 0; i < 8; ++i ) {
        BoundingBox childBounds;
        vec3 minCorner, maxCorner;

        // Manually calculate child bounds based on 'i' and parent bounds/center
        if ( !( i & 4 ) ) minCorner.x = bounds.getMin().x; else minCorner.x = center.x;
        if ( i & 4 ) maxCorner.x = bounds.getMax().x; else maxCorner.x = center.x;

        if ( !( i & 2 ) ) minCorner.y = bounds.getMin().y; else minCorner.y = center.y;
        if ( i & 2 ) maxCorner.y = bounds.getMax().y; else maxCorner.y = center.y;

        if ( !( i & 1 ) ) minCorner.z = bounds.getMin().z; else minCorner.z = center.z;
        if ( i & 1 ) maxCorner.z = bounds.getMax().z; else maxCorner.z = center.z;

        childBounds = BoundingBox( minCorner, maxCorner );
        double tNear, tFar;
        if ( rayAABBIntersect( childBounds, ray, tNear, tFar ) ) {
            t[i] = tNear;
        } else {
            t[i] = std::numeric_limits<double>::infinity();
        }
        entryOrder[i] = i;
    }

    // Sort the entry order based on the 't' values
    std::sort( entryOrder, entryOrder + 8, [&](int a, int b) { return t[a] < t[b]; } );

    // Assign the sorted order to the output
    for ( int i = 0; i < 8; ++i ) {
        outOrder[i] = entryOrder[i];
    }
}

void Octree::intersectRayNode( const OctreeNode * node, const Ray & ray, IntersectionData & closestIntersection ) const {
    if ( !node ) return; // Null node, no intersection

    double tNear, tFar;
    if ( !rayAABBIntersect( node->m_Bounds, ray, tNear, tFar ) ) {
        return; // Ray doesn't hit this node's AABB
    }

    if ( node->m_Children[0] == nullptr ) {
        for ( size_t index : node->m_TriangleIndices ) {
            const Triangle & triangle = (*m_Triangles)[index];
            IntersectionData hit = ray.triangleIntersection( triangle ); 
            if ( hit.m_Hit && hit.m_T < closestIntersection.m_T ) {
                closestIntersection = hit;
            }
        }
    } else { // Internal node, traverse children
        // Determine the order of child traversal based on ray direction
        int visitOrder[8];
        getTraversalOrder( node->m_Bounds, ray, visitOrder );

        for ( int i = 0; i < 8; ++i ) {
            intersectRayNode( node->m_Children[visitOrder[i]].get(), ray, closestIntersection );
        }
    }
}

IntersectionData Octree::intersect( const Ray & ray ) const {
    IntersectionData closestHit;
    intersectRayNode( m_Root.get(), ray, closestHit );
    return closestHit;
}
