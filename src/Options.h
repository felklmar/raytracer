#pragma once

#include <cmath>

struct Options {
    void load( size_t lightPoints, size_t maxDepth );

    size_t pointsPerLight;
    size_t maxRecursionDepth;
};

extern Options options;