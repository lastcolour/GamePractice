#include "Math/EulerAngles.hpp"

namespace Math {

EulerAngles::EulerAngles() {
}

EulerAngles::EulerAngles(float yawVal, float pitchVal, float rollVal) :
    yaw(yawVal),
    pitch(pitchVal),
    roll(rollVal) {
}

EulerAngles::EulerAngles(const EulerAngles& angles) :
    yaw(angles.yaw),
    pitch(angles.pitch),
    roll(angles.roll) {
}

EulerAngles& EulerAngles::operator=(const EulerAngles& angles) {
    yaw = angles.yaw;
    pitch = angles.pitch;
    roll = angles.roll;
    return *this;
}

EulerAngles& EulerAngles::operator+=(const EulerAngles& angles) {
    yaw += angles.yaw;
    pitch += angles.pitch;
    roll += angles.roll;
    return *this;
}

EulerAngles& EulerAngles::operator-=(const EulerAngles& angles) {
    yaw -= angles.yaw;
    pitch -= angles.pitch;
    roll -= angles.roll;
    return *this;
}

EulerAngles EulerAngles::operator-() {
    return EulerAngles(-yaw, -pitch, -roll);
}

EulerAngles& EulerAngles::operator*=(const float t) {
    yaw *= t;
    pitch = t;
    roll *= t;
    return *this;
}

EulerAngles operator*(float t, const EulerAngles& a) {
    return EulerAngles(a.yaw * t, a.pitch * t, a.roll * t);
}

EulerAngles operator*(const EulerAngles& a, float t) {
    return t * a;
}

EulerAngles operator+(const EulerAngles& a1, const EulerAngles& a2) {
    return EulerAngles(a1.yaw + a2.yaw, a1.pitch + a2.pitch, a1.roll + a2.roll);
}

EulerAngles operator-(const EulerAngles& a1, const EulerAngles& a2) {
    return EulerAngles(a1.yaw - a2.yaw, a1.pitch - a2.pitch, a1.roll - a2.roll);
}

} // namespace Math