#ifndef __PRIMITIEVS_HPP__
#define __PRIMITIEVS_HPP__

namespace Math {

const extern float PI;

template<typename T>
bool IsEqual(T a, T b) {
    return a == b;
}

template<>
bool IsEqual<float>(float a, float b);

float Deg2Rad(float degrees);

float Rad2Deg(float radians);

int RandomInt(int min, int max);

template<typename T>
T Clamp(const T& val, const T& min, const T& max) {
    if(val < min) {
        return min;
    } else if(val > max) {
        return max;
    }
    return val;
}

template<typename T>
T Lerp(const T& first, const T& second, float prog) {
    return (1.f - prog) * first +  prog * second;
}

} // namespace Math

#endif /* __PRIMITIEVS_HPP__ */