#include "math.h"

using namespace math;

SimplexNoise::SimplexNoise(int seed) : noise(seed) {
    this->noise.SetNoiseType(FastNoise::SimplexFractal);
}

SimplexNoise::SimplexNoise() : noise() {
    this->noise.SetNoiseType(FastNoise::SimplexFractal);
}

float SimplexNoise::get(float x, float y) { return this->noise.GetNoise(x, y); }