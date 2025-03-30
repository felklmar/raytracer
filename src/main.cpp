#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <cfloat>
#include <algorithm>

#include "Scene.h"
#include "Options.h"

#define WIDTH 200
#define HEIGHT 200

template <typename T>
inline T clamp( T val, T low, T high ) {
    return max( min( val, high ), low );
}

int main( int argc, char const *argv[] ) {
    if ( argc < 2 ) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        exit( EXIT_FAILURE );
    }

    options.load( 100, 1 );

    Scene scene( argv[1] );
    std::vector<vec3> colors = scene.render( WIDTH, HEIGHT );

    // Use a vector for automatic memory management
    std::vector<float> image( WIDTH * HEIGHT * 3 );
    for ( int i = 0; i < WIDTH; ++i ) {
        for ( int j = 0; j < HEIGHT; ++j ) {
            for ( int k = 0; k < 3; ++k ) {
                image[3 * (j * WIDTH + i) + k] = colors[j * WIDTH + i][k] * 255;
            }
        }
    }

    // Image saving
    std::ofstream output("output.ppm");
    output << "P3\n" << WIDTH << " " << HEIGHT << "\n" << 255 << std::endl;
    for ( int i = 0; i < WIDTH * HEIGHT * 3; ++i ) {
        output << (int)image[i] << " ";
    }
    output.close();
}
