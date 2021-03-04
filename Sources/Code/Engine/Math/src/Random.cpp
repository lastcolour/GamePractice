#include "Math/Random.hpp"

#include <cassert>

namespace Math {

RandomIntGenerator::RandomIntGenerator(int minVal, int maxVal) :
    generator(),
    distribution(minVal, maxVal) {

    assert((minVal <= maxVal) && "Invalid min val");
}

RandomIntGenerator::RandomIntGenerator() :
    RandomIntGenerator(0, 1) {
}

RandomIntGenerator::~RandomIntGenerator() {
}

void RandomIntGenerator::setRange(int minVal, int maxVal) {
    assert((minVal <= maxVal) && "Invalid min val");
    distribution = std::uniform_int_distribution<>(minVal, maxVal);
}

int RandomIntGenerator::generate() {
    return distribution(generator);
}

RandomFloatGenerator::RandomFloatGenerator(float minVal, float maxVal) :
    generator(),
    distribution(minVal, maxVal) {

    assert((minVal <= maxVal) && "Invalid min val");
}

RandomFloatGenerator::RandomFloatGenerator() :
    RandomFloatGenerator(0.f, 1.f) {
}

RandomFloatGenerator::~RandomFloatGenerator() {
}

void RandomFloatGenerator::setRange(float minVal, float maxVal) {
    assert((minVal <= maxVal) && "Invalid min val");
    distribution = std::uniform_real_distribution<>(minVal, maxVal);
}

float RandomFloatGenerator::generate() {
    return static_cast<float>(distribution(generator));
}

} // Math