#ifndef __EULER_ANGLES_HPP__
#define __EULER_ANGLES_HPP__

namespace Math {

class EulerAngles {
public:

    EulerAngles();
    EulerAngles(float yawVal, float pitchVal, float rollVal);
    EulerAngles(const EulerAngles& angles);
    EulerAngles& operator=(const EulerAngles& angles);

    EulerAngles& operator+=(const EulerAngles& angles);
    EulerAngles& operator-=(const EulerAngles& angles);
    EulerAngles operator-();
    EulerAngles& operator*=(const float t);

public:

    float yaw;
    float pitch;
    float roll;
};

EulerAngles operator*(float t, const EulerAngles& a);
EulerAngles operator*(const EulerAngles& a, float t);
EulerAngles operator+(const EulerAngles& a1, const EulerAngles& a2);
EulerAngles operator-(const EulerAngles& a1, const EulerAngles& a2);

} // namespace Math

#endif /* __EULER_ANGLES_HPP__ */