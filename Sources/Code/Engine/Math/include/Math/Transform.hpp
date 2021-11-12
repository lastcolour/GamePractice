#ifndef __TRANSFORM_HPP__
#define __TRANSFORM_HPP__

#include "Math/Vector.hpp"
#include "Math/Quaternion.hpp"

namespace Math {

class Transform {
public:

    static void Reflect(ReflectContext& ctx);

public:

    Transform();
    Transform(const Transform& tm);
    ~Transform();

    Transform& operator=(const Transform& tm);

    Mat4 toMat4() const;
    Transform getDelta(const Transform& tm) const;
    void addDelta(const Transform& tm);
    Transform getLocalDelta(const Transform& tm) const;
    void addLocalDelta(const Transform& tm);

    void inverse();
    Transform getInversed() const;

public:

    Vec3 pt;
    Vec3 scale;
    Quat quat;
};

} // namespace Math

using Transform = Math::Transform;

#endif /* __TRANSFORM_HPP__ */