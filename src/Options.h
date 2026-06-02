#pragma once

#include <cmath>

struct Options {
    void load( size_t maxDepth, size_t lightPoints, size_t octreeDepth, size_t AABBtriangles );

    size_t pointsPerLight;
    size_t maxRecursionDepth;
    size_t maxOctreeDepth;
    size_t minAABBtriangles;
};

extern Options options;