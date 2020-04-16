#ifndef SRC_RANDOMIZER_HPP_
#define SRC_RANDOMIZER_HPP_

#include <random>

class Randomizer final {
 public:
    Randomizer();
    auto get(float min, float max) -> float;

 private:
    std::mt19937 engine;
};

#endif // SRC_RANDOMIZER_HPP_
