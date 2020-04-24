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

    float Deg2rad(float degrees);

    float Rad2deg(float radians);

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
        return first * (1 - prog) + second * prog;
    }

} // namespace Math

#endif /* __PRIMITIEVS_HPP__ */