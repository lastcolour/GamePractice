#include "Math/Quaternion.hpp"

namespace Math {

Quaternion::Quaternion() {
}

Quaternion::Quaternion(const Quaternion& quat) :
    data(quat.data) {
}

Quaternion& Quaternion::operator=(const Quaternion& quat) {
    (void)quat;
    return *this;
}

Quaternion::~Quaternion() {
}

} // namespace Math