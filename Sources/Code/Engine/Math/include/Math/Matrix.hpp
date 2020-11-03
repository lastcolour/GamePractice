#ifndef __MATRIX_HPP__
#define __MATRIX_HPP__

#include "Math/Vector.hpp"

namespace Math {

class Matrix3;
class Matrix4;

class Matrix3x2 {
public:

    Matrix3x2();
    Matrix3x2(const Vec2& col0, const Vec2& col1, const Vec2& col2);
    Matrix3x2(const Matrix3x2& mat);

    Matrix3x2& operator=(const Matrix3x2& mat);
    Matrix3x2& operator+=(const Matrix3x2& mat);
    Matrix3x2& operator-=(const Matrix3x2& mat);

    Vec2& operator[](int i);
    const Vec2& operator[](int i) const;

    const float* getPtr() const;

    Matrix3 toMat3() const;

private:

    Vec2 data[3];
};

class Matrix3 {
public:

    Matrix3();
    explicit Matrix3(float val);
    Matrix3(const Vec3& col0, const Vec3& col1, const Vec3& col2);
    Matrix3(const Matrix3& mat);

    Matrix3& operator=(const Matrix3& mat);
    Matrix3& operator*=(const Matrix3& mat);
    Matrix3& operator+=(const Matrix3& mat);
    Matrix3& operator-=(const Matrix3& mat);

    Vec3& operator[](int i);
    const Vec3& operator[](int i) const;

    const float* getPtr() const;

    Matrix3x2 toMat3x2() const;

private:

    Vec3 data[3];
};

class Matrix4x3 {
public:

    Matrix4x3();
    Matrix4x3(const Vec3& col0, const Vec3& col1, const Vec3& col2, const Vec3& col3);
    Matrix4x3(const Matrix4x3& mat);

    Matrix4x3& operator=(const Matrix4x3& mat);
    Matrix4x3& operator+=(const Matrix4x3& mat);
    Matrix4x3& operator-=(const Matrix4x3& mat);

    Vec3& operator[](int i);
    const Vec3& operator[](int i) const;

    const float* getPtr() const;

    Matrix4 toMat4() const;

private:

    Vec3 data[4];
};

class Matrix4 {
public:

    Matrix4();
    explicit Matrix4(float val);
    Matrix4(const Vec4& col0, const Vec4& col1, const Vec4& col2, const Vec4& col3);
    Matrix4(const Matrix4& mat);

    Matrix4& operator=(const Matrix4& mat);
    Matrix4& operator*=(const Matrix4& mat);
    Matrix4& operator+=(const Matrix4& mat);
    Matrix4& operator-=(const Matrix4& mat);

    Vec4& operator[](int i);
    const Vec4& operator[](int i) const;

    const float* getPtr() const;

    Matrix4x3 toMat4x3() const;

private:

    Vec4 data[4];
};

// ==--------------- Mat * Vec ---------------==

Vec3 operator*(const Matrix3& m, const Vec3& v);

Vec4 operator*(const Matrix4& m, const Vec4& v);

// ==--------------- Mat * Mat ---------------==

Matrix3 operator*(const Matrix3& m1, const Matrix3& m2);

Matrix4 operator*(const Matrix4& m1, const Matrix4& m2);

// ==--------------- Mat + Mat ---------------==

Matrix3x2 operator+(const Matrix3x2& m1, const Matrix3x2& m2);

Matrix3 operator+(const Matrix3& m1, const Matrix3& m2);

Matrix4x3 operator+(const Matrix4x3& m1, const Matrix4x3& m2);

Matrix4 operator+(const Matrix4& m1, const Matrix4& m2);

// ==--------------- Mat - Mat ---------------==

Matrix3x2 operator-(const Matrix3x2& m1, const Matrix3x2& m2);

Matrix3 operator-(const Matrix3& m1, const Matrix3& m2);

Matrix4x3 operator-(const Matrix4x3& m1, const Matrix4x3& m2);

Matrix4 operator-(const Matrix4& m1, const Matrix4& m2);

} // namespace Math

typedef Math::Matrix3x2 Mat3x2;
typedef Math::Matrix3 Mat3;
typedef Math::Matrix4x3 Mat4x3;
typedef Math::Matrix4 Mat4;

#endif /* __MATRIX_HPP__ */