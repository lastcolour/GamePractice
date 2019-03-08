#ifndef __QUATERNION_HPP__
#define __QUATERNION_HPP__

#include "Math/Vector.hpp"

namespace Math {

class Quaternion {
public:

    Quaternion();
    Quaternion(const Quaternion& quat);
    Quaternion& operator=(const Quaternion& quat);
    ~Quaternion();

private:

    Vec4 data;
};

} // namespace Math

typedef Math::Quaternion Quat;

#endif /* __QUATERNION_HPP__ */