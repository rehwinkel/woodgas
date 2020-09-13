#include "math.h"

using namespace math;

SimplexNoise::SimplexNoise(int seed) : noise(seed) {
    this->noise.SetNoiseType(FastNoise::SimplexFractal);
}

SimplexNoise::SimplexNoise() : noise(0) {
    this->noise.SetNoiseType(FastNoise::SimplexFractal);
}

float SimplexNoise::get(float x, float y) { return this->noise.GetNoise(x, y); }

PseudoRandom::PseudoRandom() : rng(0), dist(-1.0f, 1.0f) {}

PseudoRandom::PseudoRandom(int seed) : rng(seed), dist(-1.0f, 1.0f) {}

float PseudoRandom::get() { return this->dist(this->rng); }