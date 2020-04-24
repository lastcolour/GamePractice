#ifndef __TRANSFORM_HPP__
#define __TRANSFORM_HPP__

#include "Math/Vector.hpp"
#include "Math/Quaternion.hpp"

namespace Math {

class Transform {
public:

    Transform();
    Transform(const Transform& tm);
    ~Transform();

    Transform& operator=(const Transform& tm);

public:

    Vec3 pt;
    Vec3 scale;
    Quat quat;
};

} // namespace Math

typedef Math::Transform Transform;

#endif /* __TRANSFORM_HPP__ */