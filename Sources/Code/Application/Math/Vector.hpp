#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

#include "Math/Primitivies.hpp"

#include <type_traits>
#include <cmath>

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

    T& operator[](int i) { return (&x)[i]; }
    T operator[](int i) const { return (&x)[i]; }

    const T* getPtr() const { return &x; }
    T getLenghtSq() const { return x * x + y * y; }
    T getLenght() const { return static_cast<T>(static_cast<double>(sqrt(getLenghtSq()))); }
    Vector2<T> getNormilized() const {
        const auto len = getLenght();
        return Vector2<T>(x / len, y / len);
    }

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
    Vector3(const Vector2<T>& v, T valZ) : x(v.x), y(v.y), z(valZ) {}

    Vector3& operator=(const Vector3& other) {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }

    ~Vector3() = default;

    T& operator[](int i) { return (&x)[i]; }
    T operator[](int i) const { return (&x)[i]; }

    const T* getPtr() const { return &x; }
    T getLenghtSq() const { return x * x + y * y + z * z; }
    T getLenght() const { return static_cast<T>(static_cast<double>(sqrt(getLenghtSq()))); }
    Vector3<T> getNormilized() const {
        const auto len = getLenght();
        return Vector3<T>(x / len, y / len, z / len);
    }

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
    Vector4(const Vector3<T>& v, T valW) : x(v.x), y(v.y), z(v.z), w(valW) {}

    Vector4& operator=(const Vector4& other) {
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
        return *this;
    }

    ~Vector4() = default;

    T& operator[](int i) { return (&x)[i]; }
    T operator[](int i) const { return (&x)[i]; }

    const T* getPtr() const { return &x; }
    T getLenghtSq() const { return x * x + y * y + z * z + w * w; }
    T getLenght() const { return static_cast<T>(static_cast<double>(sqrt(getLenghtSq()))); }
    Vector4<T> getNormilized() const {
        const auto len = getLenght();
        return Vector4(x / len, y / len, z / len, w / len);
    }

public:

    T x;
    T y;
    T z;
    T w;
};

// ==--------------- Vec + Vec ---------------==

template<typename T>
Vector2<T> operator+(const Vector2<T>& v1, const Vector2<T>& v2) {
    return Vector2<T>(v1.x + v2.x, v1.y + v2.y);
}

template<typename T>
Vector3<T> operator+(const Vector3<T>& v1, const Vector3<T>& v2) {
    return Vector3<T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

template<typename T>
Vector4<T> operator+(const Vector4<T>& v1, const Vector4<T>& v2) {
    return Vector4<T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}

// ==--------------- Vec - Vec ---------------==

template<typename T>
Vector2<T> operator-(const Vector2<T>& v1, const Vector2<T>& v2) {
    return Vector2<T>(v1.x - v2.x, v1.y - v2.y);
}

template<typename T>
Vector3<T> operator-(const Vector3<T>& v1, const Vector3<T>& v2) {
    return Vector3<T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

template<typename T>
Vector4<T> operator-(const Vector4<T>& v1, const Vector4<T>& v2) {
    return Vector4<T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
}

// ==--------------- Vec / Vec ---------------==

template<typename T>
Vector2<T> operator/(const Vector2<T>& v1, const Vector2<T>& v2) {
    return Vector2<T>(v1.x / v2.x, v1.y / v2.y);
}

template<typename T>
Vector3<T> operator/(const Vector3<T>& v1, const Vector3<T>& v2) {
    return Vector3<T>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

template<typename T>
Vector4<T> operator/(const Vector4<T>& v1, const Vector4<T>& v2) {
    return Vector4<T>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
}

// ==--------------- Vec * Scalar ---------------==

template<typename T>
Vector2<T> operator*(const Vector2<T>& v, T a) {
    return Vector2<T>(v.x * a, v.y * a);
}

template<typename T>
Vector3<T> operator*(const Vector3<T>& v, T a) {
    return Vector3<T>(v.x * a, v.y * a, v.z * a);
}

template<typename T>
Vector4<T> operator*(const Vector4<T>& v, T a) {
    return Vector4<T>(v.x * a, v.y * a, v.z * a, v.w * a);
}

// ==--------------- Vec / Scalar ---------------==

template<typename T>
Vector2<T> operator/(const Vector2<T>& v, T a) {
    return Vector2<T>(v.x / a, v.y / a);
}

template<typename T>
Vector3<T> operator/(const Vector3<T>& v, T a) {
    return Vector3<T>(v.x / a, v.y / a, v.z / a);
}

template<typename T>
Vector4<T> operator/(const Vector4<T>& v, T a) {
    return Vector4<T>(v.x / a, v.y / a, v.z / a, v.w / a);
}

// ==--------------- Scalar * Vec ---------------==

template<typename T>
Vector2<T> operator*(T a, const Vector2<T>& v) {
    return v * a;
}

template<typename T>
Vector3<T> operator*(T a, const Vector3<T>& v) {
    return v * a;
}

template<typename T>
Vector4<T> operator*(T a, const Vector4<T>& v) {
    return v * a;
}

// ==--------------- Vec != Vec ---------------==

template<typename T>
bool operator!=(const Vector2<T>& a, const Vector2<T>& b) {
    return !(a == b);
}

template<typename T>
bool operator!=(const Vector3<T>& a, const Vector3<T>& b) {
    return !(a == b);
}

template<typename T>
bool operator!=(const Vector4<T>& a, const Vector4<T>& b) {
    return !(a == b);
}

// ==--------------- Vec == Vec ---------------==

template<typename T>
bool operator==(const Vector2<T>& a, const Vector2<T>& b) {
    return IsEqual<T>(a.x, b.x) && IsEqual<T>(a.y, b.y);
}

template<typename T>
bool operator==(const Vector3<T>& a, const Vector3<T>& b) {
    return IsEqual<T>(a.x, b.x) && IsEqual<T>(a.y, b.y) && IsEqual<T>(a.z, b.z);
}

template<typename T>
bool operator==(const Vector4<T>& a, const Vector4<T>& b) {
    return IsEqual<T>(a.x, b.x) && IsEqual<T>(a.y, b.y) && IsEqual<T>(a.z, b.z) && IsEqual<T>(a.w, b.w);
}

// ==--------------- Vec > Vec ---------------==

template<typename T>
bool operator>(const Vector2<T>& v1, const Vector2<T>& v2) {
    return v1.x > v2.x && v1.y > v2.y;
}

template<typename T>
bool operator>(const Vector3<T>& v1, const Vector3<T>& v2) {
    return v1.x > v2.x && v1.y > v2.y, v1.z > v2.z;
}

template<typename T>
bool operator>(const Vector4<T>& v1, const Vector4<T>& v2) {
    return v1.x > v2.x && v1.y > v2.y && v1.z > v2.z && v1.w > v2.w;
}

// ==--------------- Vec >= Vec ---------------==

template<typename T>
bool operator>=(const Vector2<T>& v1, const Vector2<T>& v2) {
    return v1.x >= v2.x && v1.y >= v2.y;
}

template<typename T>
bool operator>=(const Vector3<T>& v1, const Vector3<T>& v2) {
    return v1.x >= v2.x && v1.y >= v2.y, v1.z >= v2.z;
}

template<typename T>
bool operator>=(const Vector4<T>& v1, const Vector4<T>& v2) {
    return v1.x >= v2.x && v1.y >= v2.y && v1.z >= v2.z && v1.w >= v2.w;
}

// ==--------------- Vec < Vec ---------------==

template<typename T>
bool operator<(const Vector2<T>& v1, const Vector2<T>& v2) {
    return v1.x < v2.x && v1.y < v2.y;
}

template<typename T>
bool operator<(const Vector3<T>& v1, const Vector3<T>& v2) {
    return v1.x < v2.x && v1.y < v2.y, v1.z < v2.z;
}

template<typename T>
bool operator<(const Vector4<T>& v1, const Vector4<T>& v2) {
    return v1.x < v2.x && v1.y < v2.y && v1.z < v2.z && v1.w < v2.w;
}

// ==--------------- Vec <= Vec ---------------==

template<typename T>
bool operator<=(const Vector2<T>& v1, const Vector2<T>& v2) {
    return v1.x <= v2.x && v1.y <= v2.y;
}

template<typename T>
bool operator<=(const Vector3<T>& v1, const Vector3<T>& v2) {
    return v1.x <= v2.x && v1.y <= v2.y, v1.z <= v2.z;
}

template<typename T>
bool operator<=(const Vector4<T>& v1, const Vector4<T>& v2) {
    return v1.x <= v2.x && v1.y <= v2.y && v1.z <= v2.z && v1.w <= v2.w;
}

} // namespace Math

typedef Math::Vector2<int> Vec2i;
typedef Math::Vector2<float> Vec2;
typedef Math::Vector3<float> Vec3;
typedef Math::Vector4<float> Vec4;

#endif /* __VECTOR_HPP__ */