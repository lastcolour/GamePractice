#include "Math/Primitivies.hpp"

#include <cassert>

namespace Math {

const float PI = 3.14159265f;

template<>
bool IsEqual<float>(float a, float b) {
    return abs(a - b) <= 0.0001f;
}

float Deg2Rad(float degrees) {
    return degrees * PI / 180.f;
}

float Rad2Deg(float radians) {
    return radians * 180.f / PI;
}

} // namespace Math