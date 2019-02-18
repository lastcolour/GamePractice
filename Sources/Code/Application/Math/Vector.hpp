#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

namespace Math {

    template<typename T>
    class Vector2 {
    public:

        Vector2() = default;
        Vector2(T valX, T valY) : x(valX), y(valY) {}
        explicit Vector2(T val) : x(val), y(val) {}
        Vector2(const Vector2& other) : x(other.x), y(other.y) {}
        Vector2& operator=(const Vector2& other) {
            x = other.x;
            y = other.y;
            return *this;
        }
        ~Vector2() = default;

    public:

        T x;
        T y;
    };

    template<typename T>
    class Vector3 {
    public:

        Vector3() = default;
        Vector3(T valX, T valY, T valZ) : x(valX), y(valY), z(valZ) {}
        explicit Vector3(T val) : x(val), y(val), z(val) {}
        Vector3(const Vector3& other) : x(other.x), y(other.y), z(other.z) {}
        Vector3& operator=(const Vector3& other) {
            x = other.x;
            y = other.y;
            z = other.z;
            return *this;
        }
        ~Vector3() = default;

    public:

        T x;
        T y;
        T z;
    };

    template<typename T>
    class Vector4 {
    public:

        Vector4() = default;
        Vector4(T valX, T valY, T valZ, T valW) : x(valX), y(valY), z(valZ), w(valW) {}
        explicit Vector4(T val) : x(val), y(val), z(val), w(val) {}
        Vector4(const Vector4& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}
        Vector4& operator=(const Vector4& other) {
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;
            return *this;
        }
        ~Vector4() = default;

    public:

        T x;
        T y;
        T z;
        T w;
    };

} // namespace Math

typedef Math::Vector2<float> Vec2;
typedef Math::Vector3<float> Vec3;
typedef Math::Vector4<float> Vec4;

#endif /* __VECTOR_HPP__ */