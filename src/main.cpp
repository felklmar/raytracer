#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <cfloat>
#include <algorithm>

#include "Scene.h"
#include "Options.h"

template <typename T>
inline T clamp( T val, T low, T high ) {
    return max( min( val, high ), low );
}

int main( int argc, char const *argv[] ) {
    if ( argc < 9 ) {
        std::cerr << "Usage: " << argv[0] << " <filename> <output-name> <img-width> <img-height> <recursion-depth> <light-samples> <octree-depth> <min-aabb-triangles>" << std::endl;
        exit( EXIT_FAILURE );
    }

    size_t width = atoi( argv[3] );
    size_t height = atoi( argv[4] );
    
    options.load( atoi( argv[5] ), atoi( argv[6] ), atoi( argv[7] ), atoi( argv[8] ) );

    Scene scene( argv[1] );
    std::vector<vec3> colors = scene.render( width, height );

    // Use a vector for automatic memory management
    std::vector<float> image( width * height * 3 );
    for ( size_t i = 0; i < width; ++i ) {
        for ( size_t j = 0; j < height; ++j ) {
            for ( int k = 0; k < 3; ++k ) {
                image[3 * (j * width + i) + k] = colors[j * width + i][k] * 255;
            }
        }
    }

    // Image saving
    std::ofstream output( argv[2] );
    output << "P3\n" << width << " " << height << "\n" << 255 << std::endl;
    for ( size_t i = 0; i < width * height * 3; ++i ) {
        output << (int)image[i] << " ";
    }
    output.close();
}
