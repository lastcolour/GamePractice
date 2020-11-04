#ifndef __QUATERNION_HPP__
#define __QUATERNION_HPP__

#include "Math/Matrix.hpp"
#include "EulerAngles.hpp"

namespace Math {

class Quaternion {
public:

    static Quaternion Indentity();

public:

    Quaternion();
    Quaternion(float valX, float valY, float valZ, float valW);
    Quaternion(const Quaternion& quat);
    Quaternion& operator=(const Quaternion& quat);
    ~Quaternion();

    void setIndentity();
    void setEulerAngles(const EulerAngles& angles);
    void setAxisAngle(const Vec3& axis, float angle);
    void setLookAtRotation(const Vec3& dir, const Vec3& up);
    void setRotationBetween(const Vec3& p1, const Vec3& p2);
    void setRotationMat(const Mat3& mat);
    void setRotationMat(const Mat4& mat);

    EulerAngles getEulerAngels() const;
    void getAxisAngle(Vec3& outAxis, float& outAngle) const;
    float getLenght() const;
    float getLenghtSq() const;
    Quaternion getNormalized() const;
    Quaternion getInversed() const;
    Quaternion getConjugated() const;

    void normilize();
    void inverse();
    void conjugate();

    Mat4 toMat4() const;
    Mat3 toMat3() const;

    Quaternion& operator*=(const Quaternion& q);
    Quaternion& operator+=(const Quaternion& q);
    Quaternion& operator-=(const Quaternion& q);
    Quaternion operator-();

public:

    float x;
    float y;
    float z;
    float w;
};

Quaternion operator*(float a, const Quaternion& q);

Quaternion operator*(const Quaternion& q1, const Quaternion& q2);

Vec3 operator*(const Quaternion& q, const Vec3& pt);

Quaternion operator+(const Quaternion& q1, const Quaternion& q2);

Quaternion operator-(const Quaternion& q1, const Quaternion& q2);

Quaternion SLerp(const Quaternion& q1, const Quaternion& q2, float t);

Quaternion NLerp(const Quaternion& q1, const Quaternion& q2, float t);

} // namespace Math

using Quat = Math::Quaternion;

#endif /* __QUATERNION_HPP__ */