#include "Randomizer.hpp"

#include <random>

Randomizer::Randomizer()
{
    std::random_device dev;
    std::mt19937 engine(dev());
    this->engine = engine;
}

auto Randomizer::get(float min, float max) -> float
{
    std::uniform_real_distribution<float> dis(min, max);
    return dis(this->engine);
}
