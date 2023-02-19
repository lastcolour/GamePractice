#include "Math/Primitivies.hpp"

#include <cassert>

namespace Math {

const float PI = 3.14159265f;

bool IsEqual(float a, float b, float eps) {
    return abs(a - b) <= eps;
}

float Deg2Rad(float degrees) {
    return degrees * PI / 180.f;
}

float Rad2Deg(float radians) {
    return radians * 180.f / PI;
}

} // namespace Math