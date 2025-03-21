#include "Options.h"

Options options; 

void Options::load( size_t lightPoints, size_t maxDepth ) {
    pointsPerLight = lightPoints;
    maxRecursionDepth = maxDepth;
}