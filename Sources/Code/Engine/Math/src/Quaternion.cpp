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

void Quaternion::setEulerAngles(const EulerAngles& angles) {
    float halfY = angles.yaw * 0.5f;
    float halfP = angles.pitch * 0.5f;
    float halfR = angles.roll * 0.5f;

    float cy = cos(halfY);
    float sy = sin(halfY);
    float cp = cos(halfP);
    float sp = sin(halfP);
    float cr = cos(halfR);
    float sr = sin(halfR);

    w = cr * cp * cy + sr * sp * sy;
    x = sr * cp * cy - cr * sp * sy;
    y = cr * sp * cy + sr * cp * sy;
    z = cr * cp * sy - sr * sp * cy;
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
    Vec3 forwardDir = dir.getNormalized();
    Vec3 rightDir = up.getNormalized().cross(forwardDir);
    Vec3 upDir = forwardDir.cross(rightDir);
    return setRotationMat(Mat3(rightDir, upDir, forwardDir));
}

void Quaternion::setRotationBetween(const Vec3& p1, const Vec3& p2) {
    float cosVal = p1.dot(p2);
    float t = sqrt(p1.lengthSq() * p1.lengthSq());

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

void Quaternion::setRotationMat(const Mat4x3& mat) {
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

EulerAngles Quaternion::getEulerAngels() const {
    EulerAngles angles;
    {
        float sinA = 2 * (w * z + x * y);
        float cosA = 1 - 2 * (y * y + z * z);
        angles.yaw = atan2(sinA, cosA);
    }
    {
        float sinA = 2.f * (w * y - z * x);
        if(abs(sinA) >= 1) {
            angles.pitch = copysign(Math::PI / 2.f, sinA);
        } else {
            angles.pitch = asin(sinA);
        }
    }
    {
        float sinA = 2.f * (w * x + y * z);
        float cosA = 1.f - 2.f * (x * x + y * y);
        angles.roll = atan2(sinA, cosA);
    }

    return angles;
}

void Quaternion::getAxisAngle(Vec3& outAxis, float& outAngle) const {
    float s = sqrt(1.f - w * w);
    if(s > 0.0001f) {
        outAngle = 2.f * acos(w);
        outAxis.x = x / s;
        outAxis.y = y / s;
        outAxis.z = z / s;
        outAxis.normalize();
    } else {
        outAngle = 0.f;
        outAxis.x = 0.f;
        outAxis.y = 0.f;
        outAxis.z = 1.f;
    }
}

float Quaternion::length() const {
    return sqrt(lengthSq());
}

float Quaternion::lengthSq() const {
    return x * x + y * y + z * z + w * w;
}

Quaternion Quaternion::getNormalized() const {
    float len = length();
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

Quaternion operator*(float a, const Quaternion& q) {
    return Quaternion(q.x * a, q.y * a, q.z * a, q.w * a);
}

Quaternion operator*(const Quaternion& q1, const Quaternion& q2) {
    Quaternion resQuat;

    resQuat.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    resQuat.y = q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z;
    resQuat.z = q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x;
    resQuat.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;

    return resQuat;
}

Quaternion& Quaternion::operator*=(const Quaternion& q) {
    *this = *this * q;
    return *this;
}

Quaternion& Quaternion::operator+=(const Quaternion& q) {
    *this = *this + q;
    return *this;
}

Quaternion& Quaternion::operator-=(const Quaternion& q) {
    *this = *this - q;
    return *this;
}

Quaternion Quaternion::operator-() {
    Quaternion q;
    q.x = -x;
    q.y = -y;
    q.z = -z;
    q.w = -w;
    return q;
}

Quaternion operator+(const Quaternion& q1, const Quaternion& q2) {
    return Quaternion(q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w);
}

Quaternion operator-(const Quaternion& q1, const Quaternion& q2) {
    return Quaternion(q1.x - q2.x, q1.y - q2.y, q1.z - q2.z, q1.w - q2.w);
}

Vec3 operator*(const Quaternion& q, const Vec3& pt) {
    Vec3 u(q.x, q.y, q.z);
    return 2.0f * u.dot(pt) * u + (q.w * q.w - u.dot(u)) * pt + 2.0f * q.w * u.cross(pt);
}

Quaternion SLerp(const Quaternion& q1, const Quaternion& q2, float t) {
    float cosA = Dot(q1, q2);

    float val2Sign = 1.f;
    if(cosA < 0.f) {
        cosA = -cosA;
        val2Sign = -1.f;
    }

    float val1;
    float val2;

    if(cosA > 0.995f) {
        val1 = 1.f - t;
        val2 = t;
    } else {
        float angle = acos(cosA);
        float sinA = 1.f / sin(angle);
        val1 = sin((1.f - t) * angle) * sinA;
        val2 = sin(t * angle) * sinA;
    }

    val2 *= val2Sign;

    return val1 * q1 + val2 * q2;
}

Quaternion NLerp(const Quaternion& q1, const Quaternion& q2, float t) {
    float valSign = 1.f;
    if(Dot(q1, q2) < 0.f) {
        valSign = -1.f;
    }
    Quaternion res = Math::Lerp(q1, valSign * q2, t);
    res.normilize();
    return res;
}

} // namespace Math