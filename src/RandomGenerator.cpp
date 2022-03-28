#include "RandomGenerator.h"

RandomGenerator::RandomGenerator() : engine(std::random_device()()) {};

int RandomGenerator::randInt(int l, int r)
{
    return std::uniform_int_distribution(l, r)(engine);
}
