#include "Math/Transform.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Math/MatrixTransform.hpp"

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

} // namespace Math