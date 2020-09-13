// header for various mathematical functions for convenience

#include <FastNoise.h>

namespace math {
    class SimplexNoise {
        FastNoise noise;

       public:
        SimplexNoise();
        SimplexNoise(int seed);
        float get(float x, float y);
    };
}