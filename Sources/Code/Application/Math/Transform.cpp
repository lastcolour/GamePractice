#include "Math/Transform.hpp"

namespace Math {

Transform::Transform() :
    pt(0.f),
    scale(1.f),
    quat() {
}

Transform::Transform(const Transform& tm) : 
    pt(tm.pt),
    scale(tm.scale),
    quat(tm.quat) {
}

Transform::~Transform() {
}

Transform& Transform::operator=(const Transform& tm) {
    if(this != &tm) {
        pt = tm.pt;
        scale = tm.scale;
        quat = tm.quat;
    }
    return *this;
}

} // namespace Math