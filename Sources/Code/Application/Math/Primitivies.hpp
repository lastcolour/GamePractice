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

} // namespace Math

#endif /* __PRIMITIEVS_HPP__ */