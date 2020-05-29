#include "Math/Quaternion.hpp"

namespace Math {

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

} // namespace Math