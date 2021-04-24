#include "Math/MatrixTransform.hpp"
#include "Math/Transform.hpp"
#include "Reflect/ReflectContext.hpp"

namespace {

Vec3 getSafeScale(const Vec3& scale) {
    Vec3 s;
    s.x = std::max(0.001f, scale.x);
    s.y = std::max(0.001f, scale.y);
    s.z = std::max(0.001f, scale.z);
    return s;
}

} // namepsace

namespace Math {

void Transform::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<Transform>("Transform")) {
        classInfo->addField("pos", &Transform::pt);
        classInfo->addField("scale", &Transform::scale);
        classInfo->addField("rot", &Transform::quat);
    }
}

Transform::Transform() :
    pt(0.f),
    scale(1.f),
    quat(Quaternion::Indentity()) {
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

Mat4 Transform::toMat4() const {
    Mat4 resMat(1.f);
    AddTranslate(resMat, pt);
    AddRotate(resMat, quat);
    AddScale(resMat, scale);
    return resMat;
}

Transform Transform::getDelta(const Transform& tm) const {
    Vec3 safeScale = getSafeScale(tm.scale);

    Transform deltaTm;
    deltaTm.pt = pt - tm.pt;
    deltaTm.scale.x = scale.x / safeScale.x;
    deltaTm.scale.y = scale.y / safeScale.y;
    deltaTm.scale.z = scale.z / safeScale.z;
    deltaTm.quat = tm.quat.getInversed() * quat;
    return deltaTm;
}

void Transform::addDelta(const Transform& tm) {
    pt += tm.pt;
    scale.x *= tm.scale.x;
    scale.y *= tm.scale.y;
    scale.z *= tm.scale.z;
    quat *= tm.quat;
}

Transform Transform::getLocalDelta(const Transform& tm) const {
    Quat qInv = quat.getInversed();

    Vec3 safeScale = getSafeScale(scale);

    Transform deltaTm;
    deltaTm.pt = qInv * (tm.pt - pt);
    deltaTm.pt.x /= safeScale.x;
    deltaTm.pt.y /= safeScale.y;
    deltaTm.pt.z /= safeScale.z;

    deltaTm.scale.x = tm.scale.x / safeScale.x;
    deltaTm.scale.y = tm.scale.y / safeScale.y;
    deltaTm.scale.z = tm.scale.z / safeScale.z;
    
    deltaTm.quat = qInv * tm.quat;

    return deltaTm;
}

void Transform::addLocalDelta(const Transform& tm) {
    pt += quat * tm.pt.getScaled(scale);
    scale = tm.scale.getScaled(scale);
    quat *= tm.quat;
}


void Transform::inverse() {
    Vec3 safeScale = getSafeScale(scale);

    pt = -pt;
    scale.x = 1.f / safeScale.x;
    scale.y = 1.f / safeScale.y;
    scale.z = 1.f / safeScale.z;
    quat.inverse();
}

Transform Transform::getInversed() const {
    Transform tm = *this;
    tm.inverse();
    return tm;
}

} // namespace Math