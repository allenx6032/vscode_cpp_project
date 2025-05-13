/**
 * @Author: Amélie Heinrich
 * @Create Time: 2024-03-23 17:41:33
 */

#pragma once

#include <ctime>
#include <random>

class RNG
{
public:
    static RNG pRNG;

    RNG(int seed = std::time(nullptr))
        : rng(dev())
    {
    }

    int GetIntInRange(int low, int high)
    {
        std::uniform_int_distribution<int> dist(low, high);

        return dist(rng);
    }

    std::random_device dev;
    std::mt19937 rng;
};
