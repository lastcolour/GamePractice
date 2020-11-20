#ifndef __RANDOM_HPP__
#define __RANDOM_HPP__

#include <random>

namespace Math {

class RandomIntGenerator {
public:

    RandomIntGenerator(int minVal, int maxVal);
    RandomIntGenerator();
    ~RandomIntGenerator();

    void setRange(int minVal, int maxVal);
    int generate();

private:

    std::mt19937 generator;
    std::uniform_int_distribution<> distribution;
};

class RandomFloatGenerator {
public:

    RandomFloatGenerator(float minVal, float maxVal);
    RandomFloatGenerator();
    ~RandomFloatGenerator();

    void setRange(float minVal, float maxVal);
    float generate();

private:

    std::mt19937 generator;
    std::uniform_real_distribution<> distribution;
};

} // namespace Math

#endif /* __RANDOM_HPP__ */