#include "Math/Quaternion.hpp"

namespace Math {

Quaternion Quaternion::Indentity() {
    return Quaternion(0.f, 0.f, 0.f, 1.f);
}

Quaternion::Quaternion() {
}

Quaternion::Quaternion(float valX, float valY, float valZ, float valW) :
    x(valX), y(valY), z(valZ), w(valW) {
}

Quaternion::Quaternion(const Quaternion& quat) :
    x(quat.x), y(quat.y), z(quat.z), w(quat.w) {
}

Quaternion& Quaternion::operator=(const Quaternion& quat) {
    x = quat.x;
    y = quat.y;
    z = quat.z;
    w = quat.w;
    return *this;
}

Quaternion::~Quaternion() {
}

void Quaternion::setEurelAnges(const Vec3& angles) {
}

void Quaternion::setAxisAngle(const Vec3& axis, float angle) {
}

void Quaternion::setLookAtRotation(const Vec3& dir, const Vec3& up) {
}

void Quaternion::setRotationBetween(const Vec3& p1, const Vec3& p2) {
}

void Quaternion::setRotationMat(const Mat3& mat) {
    float trace = mat[0][0] + mat[1][1] + mat[2][2];
    if(trace > 0.f) {
        float s = 0.5f / sqrtf(trace + 1.0f);
        w = 0.25f / s;
        x = (mat[1][2] - mat[2][1]) * s;
        y = (mat[2][0] - mat[0][2]) * s;
        z = (mat[0][1] - mat[1][0]) * s;
    } else {
        if(mat[0][0] > mat[1][1] && mat[0][0] > mat[2][2]) {
            float s = 2.0f * sqrtf(1.0f + mat[0][0] - mat[1][1] - mat[2][2]);
            w = (mat[1][2] - mat[2][1]) / s;
            x = 0.25f * s;
            y = (mat[1][0] + mat[0][1]) / s;
            z = (mat[2][0] + mat[0][2]) / s;
        } else if(mat[1][1] > mat[2][2]) {
            float s = 2.0f * sqrtf(1.0f + mat[1][1] - mat[0][0] - mat[2][2]);
            w = (mat[2][0] - mat[0][2]) / s;
            x = (mat[1][0] + mat[0][1]) / s;
            y = 0.25f * s;
            z = (mat[2][1] + mat[1][2]) / s;
        } else {
            float s = 2.0f * sqrtf(1.0f + mat[2][2] - mat[0][0] - mat[1][1]);
            w = (mat[0][1] - mat[1][0]) / s;
            x = (mat[2][0] + mat[0][2]) / s;
            y = (mat[2][1] + mat[1][2]) / s;
            z = 0.25f * s;
        }
    }
}

Vec3 Quaternion::getEurelAngels() const {
    return Vec3();
}

void Quaternion::getAxisAngle(Vec3& outAxis, float& outAngle) const {
}

float Quaternion::getLenght() const {
    return sqrt(getLenghtSq());
}

float Quaternion::getLenghtSq() const {
    return x * x + y * y + z * z + w * w;
}

Quaternion Quaternion::getNormalized() const {
    float len = getLenght();
    return Quaternion(x / len, y / len, z / len, w / len);
}

void Quaternion::normilize() {
    *this = getNormalized();
}

Mat4 Quaternion::toMat4() const {
    auto mat3 = toMat3();
    return Mat4(
        Vec4(mat3[0], 0.f),
        Vec4(mat3[1], 0.f),
        Vec4(mat3[2], 0.f),
        Vec4(0.f, 0.f, 0.f, 1.f));
}

Mat3 Quaternion::toMat3() const {
    float xx = x * x;
    float xy = x * y;
    float xz = x * z;
    float xw = x * w;

    float yy = y * y;
    float yz = y * z;
    float yw = y * w;

    float zz = z * z;
    float zw = z * w;

    Vec3 col0(
        1.f - 2.f * (yy + zz),
        2.f * (xy - zw),
        2 * (xz + yw));

    Vec3 col1(
        2.f * (xy + zw),
        1.f - 2.f * (xx + zz),
        2.f * (yz - xw));

    Vec3 col2(
        2.f * (xz - yw),
        2.f * (yz + xw),
        1.f - 2.f * (xx + yy));

    return Mat3(col0, col1, col2);
}

Quaternion operator*(const Quaternion& q1, const Quaternion& q2) {
    Quaternion resQuat;
    return resQuat;
}

Vec3 operator*(const Quaternion& q, const Vec3& pt) {
    Vec3 resPt;
    return resPt;
}

} // namespace Math