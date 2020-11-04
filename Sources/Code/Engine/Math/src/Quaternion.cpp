#include "Math/Quaternion.hpp"

namespace {

Vec3 getOrthogonal(const Vec3& v) {
    return v;
}

float Dot(const Quat& a, const Quat& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

} // namespace

namespace Math {

Quaternion Quaternion::Indentity() {
    Quaternion q;
    q.setIndentity();
    return q;
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

void Quaternion::setEulerAngles(const Vec3& angles) {
}

void Quaternion::setIndentity() {
    x = 0.f;
    y = 0.f;
    z = 0.f;
    w = 1.f;
}

void Quaternion::setAxisAngle(const Vec3& axis, float angle) {
    float halfAngle = angle / 2.f;
    float sinVal = sin(halfAngle);
    float cosVal = cos(halfAngle);
    x = axis.x * sinVal;
    y = axis.y * sinVal;
    z = axis.z * sinVal;
    w = cosVal;
}

void Quaternion::setLookAtRotation(const Vec3& dir, const Vec3& up) {
    Vec3 forwardDir = dir.getNormilized();
    Vec3 rightDir = up.getNormilized().cross(forwardDir);
    Vec3 upDir = forwardDir.cross(rightDir);
    return setRotationMat(Mat3(rightDir, upDir, forwardDir));
}

void Quaternion::setRotationBetween(const Vec3& p1, const Vec3& p2) {
    float cosVal = p1.dot(p2);
    float t = sqrt(p1.getLenghtSq() * p1.getLenghtSq());

    if(IsEqual(t, 0.f)) {
        setIndentity();
    } else if (IsEqual(cosVal / t, -1.f)) {
        Vec3 ortAxis = getOrthogonal(p1);
        x = ortAxis.x;
        y = ortAxis.y;
        z = ortAxis.y;
        w = 0.f;
    } else {
        Vec3 axis = p1.cross(p2);
        x = axis.x;
        y = axis.y;
        z = axis.z;
        w = cosVal + t;
        normilize();
    }
}

void Quaternion::setRotationMat(const Mat4& mat) {
    setRotationMat(Mat3(
        Vec3(mat[0][0], mat[0][1], mat[0][2]),
        Vec3(mat[1][0], mat[1][1], mat[1][2]),
        Vec3(mat[2][0], mat[2][1], mat[2][2])));
}

void Quaternion::setRotationMat(const Mat3& mat) {
    float trace = mat[0][0] + mat[1][1] + mat[2][2] + 1.f;
    if(trace > 0.f) {
        float s = 0.5f / sqrt(trace);
        w = 0.25f / s;
        x = (mat[1][2] - mat[2][1]) * s;
        y = (mat[2][0] - mat[0][2]) * s;
        z = (mat[0][1] - mat[1][0]) * s;
    } else {
        if(mat[0][0] > mat[1][1] && mat[0][0] > mat[2][2]) {
            float s = 0.5f / sqrt(1.0f + mat[0][0] - mat[1][1] - mat[2][2]);
            w = (mat[1][2] - mat[2][1]) * s;
            x = 0.25f / s;
            y = (mat[1][0] + mat[0][1]) * s;
            z = (mat[2][0] + mat[0][2]) * s;
        } else if(mat[1][1] > mat[2][2]) {
            float s = 0.5f / sqrt(1.0f + mat[1][1] - mat[0][0] - mat[2][2]);
            w = (mat[2][0] - mat[0][2]) * s;
            x = (mat[1][0] + mat[0][1]) * s;
            y = 0.25f / s;
            z = (mat[2][1] + mat[1][2]) * s;
        } else {
            float s = 0.5f / sqrt(1.0f + mat[2][2] - mat[0][0] - mat[1][1]);
            w = (mat[0][1] - mat[1][0]) * s;
            x = (mat[2][0] + mat[0][2]) * s;
            y = (mat[2][1] + mat[1][2]) * s;
            z = 0.25f / s;
        }
    }
}

Vec3 Quaternion::getEulerAngels() const {
    return Vec3();
}

void Quaternion::getAxisAngle(Vec3& outAxis, float& outAngle) const {
    float s = sqrt(1.f - w * w);
    if(s > 0.0001f) {
        outAngle = 2.f * acos(w);
        outAxis.x = x / s;
        outAxis.y = y / s;
        outAxis.z = z / s;
    } else {
        outAngle = 0.f;
        outAxis.x = 0.f;
        outAxis.y = 0.f;
        outAxis.z = 1.f;
    }
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

Quaternion Quaternion::getInversed() const {
    return getConjugated();
}

Quaternion Quaternion::getConjugated() const {
    return Quaternion(-x, -y, -z, w);
}

void Quaternion::normilize() {
    *this = getNormalized();
}

void Quaternion::inverse() {
    *this = getInversed();
}

void Quaternion::conjugate() {
    *this = getConjugated();
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

    float x2 = x + x;
    float y2 = y + y;
    float z2 = z + z;

    float xx2 = x * x2;
    float yy2 = y * y2;
    float zz2 = z * z2;
    float xy2 = x * y2;
    float xz2 = x * z2;
    float yz2 = y * z2;
    float wx2 = w * x2;
    float wy2 = w * y2;
    float wz2 = w * z2;

    Mat3 rotMat;

    rotMat[0][0] = 1.0f - yy2 - zz2;
    rotMat[0][1] = xy2 + wz2;
    rotMat[0][2] = xz2 - wy2;

    rotMat[1][0] = xy2 - wz2;
    rotMat[1][1] = 1.0f - xx2 - zz2;
    rotMat[1][2] = yz2 + wx2;

    rotMat[2][0] = xz2 + wy2;
    rotMat[2][1] = yz2 - wx2;
    rotMat[2][2] = 1.0f - xx2 - yy2;

    return rotMat;
}

Quaternion operator*(const Quaternion& q1, const Quaternion& q2) {
    Quaternion resQuat;

    resQuat.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    resQuat.y = q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z;
    resQuat.z = q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x;
    resQuat.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;

    return resQuat;
}

Vec3 operator*(const Quaternion& q, const Vec3& pt) {
    Vec3 u(q.x, q.y, q.z);
    return 2.0f * u.dot(pt) * u + (q.w * q.w - u.dot(u)) * pt + 2.0f * q.w * u.cross(pt);
}

Quaternion SLerp(const Quaternion& q1, const Quaternion& q2, float t) {

    float cosVal = Dot(q1, q2);
    if(cosVal > 0.995f) {
        return NLerp(q1, q2, t);
    }
    if(cosVal < 0.f) {
        cosVal -= cosVal;
    }

    return Quaternion();
}

Quaternion NLerp(const Quaternion& q1, const Quaternion& q2, float t) {
    Quaternion tq;

    if(Dot(q1, q2) < 0.f) {
        tq.x = -q2.x;
        tq.y = -q2.y;
        tq.z = -q2.z;
        tq.w = -q2.w;
    } else {
        tq = q2;
    }

    Quaternion resQ;
    resQ.x = Lerp(q1.x, tq.x, t);
    resQ.y = Lerp(q1.y, tq.y, t);
    resQ.z = Lerp(q1.z, tq.z, t);
    resQ.w = Lerp(q1.w, tq.w, t);
    resQ.normilize();

    return resQ;
}

} // namespace Math