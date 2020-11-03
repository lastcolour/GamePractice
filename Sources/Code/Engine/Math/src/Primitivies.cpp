#include "Math/Primitivies.hpp"

#include <cmath>
#include <random>
#include <cassert>

namespace Math {

const float PI = 3.14159265f;

template<>
bool IsEqual<float>(float a, float b) {
    return fabs(a - b) <= 0.0001f;
}

float Deg2rad(float degrees) {
    return degrees * PI / 180.f;
}

float Rad2deg(float radians) {
    return radians * 180.f / PI;
}

int RandomInt(int min, int max) {
    assert(min <= max && "Invalid range for random number");

    std::random_device randDevice;
    std::mt19937 generator(randDevice());
    std::uniform_int_distribution<> distribution(min, max);

    return distribution(generator);
}

} // namespace Math