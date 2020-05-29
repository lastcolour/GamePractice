#ifndef __QUATERNION_HPP__
#define __QUATERNION_HPP__

#include "Math/Vector.hpp"

namespace Math {

class Quaternion {
public:

    Quaternion();
    Quaternion(float valX, float valY, float valZ, float valW);
    Quaternion(const Quaternion& quat);
    Quaternion& operator=(const Quaternion& quat);
    ~Quaternion();

public:

    float x;
    float y;
    float z;
    float w;
};

} // namespace Math

typedef Math::Quaternion Quat;

#endif /* __QUATERNION_HPP__ */