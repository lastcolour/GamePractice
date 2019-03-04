#include "Math/Primitivies.hpp"

#include <cmath>

namespace Math {

const float PI = 3.14159265f;

template<>
bool IsEqual<float>(float a, float b) {
    return fabs(a - b) <= 0.001f;
}

float Deg2rad(float degrees) {
    return degrees * PI / 180.f;
}

float Rad2deg(float radians) {
    return radians * 180.f / PI;
}

} // namespace Math