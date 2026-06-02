#include "Options.h"

Options options; 

void Options::load( size_t maxDepth, size_t lightPoints, size_t octreeDepth, size_t AABBtriangles ) {
    pointsPerLight = lightPoints;
    maxRecursionDepth = maxDepth;
    maxOctreeDepth = octreeDepth;
    minAABBtriangles = AABBtriangles;
}