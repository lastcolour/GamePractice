#include "Math/Quaternion.hpp"

namespace Math {

Quaternion::Quaternion() : x(0.f), y(0.f), z(0.f), w(1.f) {
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

} // namespace Math