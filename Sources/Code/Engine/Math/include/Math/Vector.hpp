#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

#include "Math/Primitivies.hpp"

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

    Vector2& operator-=(const Vector2& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    Vector2& operator+=(const Vector2& v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vector2& operator*=(const T a) {
        x *= a;
        y *= a;
        return *this;
    }

    Vector2& operator/=(const T a) {
        x /= a;
        y /= a;
        return *this;
    }

    Vector2 operator-() {
        return Vector2(-x, -y);
    }

    const T* getPtr() const { return &x; }

    T getLenghtSq() const { return x * x + y * y; }

    T getLenght() const { return static_cast<T>(static_cast<float>(sqrt(getLenghtSq()))); }

    Vector2<T> getNormilized() const {
        const auto len = getLenght();
        return Vector2<T>(x / len, y / len);
    }

    void normalize() {
        *this = getNormilized();
    }

    void scale(const Vector2& v) {
        x *= v.x;
        y *= v.y;
    }

    void scale(float sX, float sY) {
        x = static_cast<T>(x * sX);
        y = static_cast<T>(y * sY);
    }

    Vector2 getScaled(const Vector2& v) const {
        return Vector2(x * v.x, y * v.y);
    }

    Vector2 getScaled(float sX, float sY) const {
        return Vector2(static_cast<T>(x * sX), static_cast<T>(y * sY));
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

    Vector3& operator-=(const Vector3& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vector3& operator+=(const Vector3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vector3& operator*=(const T a) {
        x *= a;
        y *= a;
        z *= a;
        return *this;
    }

    Vector3& operator/=(const T a) {
        x /= a;
        y /= a;
        z /= a;
        return *this;
    }

    Vector3 operator-() {
        return Vector3(-x, -y, -z);
    }

    const T* getPtr() const { return &x; }

    T getLenghtSq() const { return x * x + y * y + z * z; }

    T getLenght() const { return static_cast<T>(static_cast<float>(sqrt(getLenghtSq()))); }

    Vector3<T> getNormilized() const {
        const auto len = getLenght();
        return Vector3<T>(x / len, y / len, z / len);
    }

    void normalize() {
        *this = getNormilized();
    }

    void scale(const Vector3& v) {
        x *= v.x;
        y *= v.y;
        z *= v.z;
    }

    void scale(float sX, float sY, float sZ) {
        x = static_cast<T>(x * sX);
        y = static_cast<T>(y * sY);
        z = static_cast<T>(z * sZ);
    }

    Vector3 getScaled(const Vector3& v) const {
        return Vector3(x * v.x, y * v.y, z * v.z);
    }

    Vector3 getScaled(float sX, float sY, float sZ) const {
        return Vector3(static_cast<T>(x * sX), static_cast<T>(y * sY), static_cast<T>(z * sZ));
    }

    float dot(const Vector3& v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    Vector3 cross(const Vector3& v) const {
        return Vector3(
            y * v.z - v.y * z,
            z * v.x - v.z * x,
            x * v.y - v.x * y);
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

    Vector4& operator-=(const Vector4& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
        return *this;
    }

    Vector4& operator+=(const Vector4& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
        return *this;
    }

    Vector4& operator*=(const T a) {
        x *= a;
        y *= a;
        z *= a;
        w *= a;
        return *this;
    }

    Vector4& operator/=(const T a) {
        x /= a;
        y /= a;
        z /= a;
        w /= a;
        return *this;
    }

    Vector4 operator-() {
        return Vector4(-x, -y, -z, -w);
    }

    const T* getPtr() const { return &x; }

    T getLenghtSq() const { return x * x + y * y + z * z + w * w; }

    T getLenght() const { return static_cast<T>(static_cast<float>(sqrt(getLenghtSq()))); }

    Vector4<T> getNormilized() const {
        const auto len = getLenght();
        return Vector4(x / len, y / len, z / len, w / len);
    }

    void normalize() {
        *this = getNormilized();
    }

    void scale(const Vector4& v) {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        w *= v.w;
    }

    void scale(float sX, float sY, float sZ, float sW) {
        x = static_cast<T>(x * sX);
        y = static_cast<T>(y * sY);
        z = static_cast<T>(z * sZ);
        w = static_cast<T>(w * sW);
    }

    Vector4 getScaled(const Vector4& v) const {
        return Vector4(x * v.x, y * v.y, z * v.z, w * v.w);
    }

    Vector4 getScaled(float sX, float sY, float sZ, float sW) const {
        return Vector4(static_cast<T>(x * sX), static_cast<T>(y * sY),
            static_cast<T>(z * sZ), static_cast<T>(w * sW));
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