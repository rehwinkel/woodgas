// header for various mathematical functions for convenience

#include <FastNoise.h>
#include <random>

namespace math {
    class SimplexNoise {
        FastNoise noise;

       public:
        SimplexNoise();
        SimplexNoise(int seed);
        float get(float x, float y);
    };

    class PseudoRandom {
        std::mt19937 rng;
        std::uniform_real_distribution<float> dist;

       public:
        PseudoRandom();
        PseudoRandom(int seed);
        float get();
    };

    template <class T>
    inline void hash_combine(std::size_t &seed, const T &v) {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
}