#ifndef __QUATERNION_HPP__
#define __QUATERNION_HPP__

#include "Math/Matrix.hpp"

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

    void setEurelAnges(const Vec3& angles);
    void setAxisAngle(const Vec3& axis, float angle);
    void setLookAtRotation(const Vec3& dir, const Vec3& up);
    void setRotationBetween(const Vec3& p1, const Vec3& p2);
    void setRotationMat(const Mat3& mat);

    Vec3 getEurelAngels() const;
    void getAxisAngle(Vec3& outAxis, float& outAngle) const;
    float getLenght() const;
    float getLenghtSq() const;
    Quaternion getNormalized() const;

    void normilize();

    Mat4 toMat4() const;
    Mat3 toMat3() const;

public:

    float x;
    float y;
    float z;
    float w;
};

Quaternion operator*(const Quaternion& q1, const Quaternion& q2);

Vec3 operator*(const Quaternion& q, const Vec3& pt);

} // namespace Math

typedef Math::Quaternion Quat;

#endif /* __QUATERNION_HPP__ */