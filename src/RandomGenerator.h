#pragma once

#include <random>

class RandomGenerator
{
public:
    RandomGenerator();

    int randInt(int l, int r);

private:
    std::mt19937 engine;
};