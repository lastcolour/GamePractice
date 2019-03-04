#include "Math/Transform.hpp"
#include "Math/Primitivies.hpp"

#include <cmath>

namespace Math {

void ProjectOrtho(Mat4& tm, float left, float right, float bot, float top, float zNear, float zFar) {
    tm = Mat4(1.f);
    tm[0][0] = 2.f / (right - left);
    tm[1][1] = 2.f / (top - bot);
    tm[2][2] = -2.f / (zFar - zNear);
    tm[3][0] = -(right + left) / (right - left);
    tm[3][1] = -(top + bot) / (top - bot);
    tm[3][2] = -(zFar + zNear) / (zFar - zNear);
}

void Translate(Mat4& tm, const Vec3& pos) {
     tm[3] = tm[0] * pos[0] + tm[1] * pos[1] + tm[2] * pos[2] + tm[3];
}

void Rotate(Mat4& tm, float angle, const Vec3& rotAxis) {
    const float ang = Deg2rad(angle);
    const float cosA = cos(ang);
    const float sinA = sin(ang);
    const Vec3 axis = rotAxis.getNormilized();
    const Vec3 temp = (1.f - cosA) * axis;
    Mat4 rotTm;

    Mat4 rotMat;
    rotMat[0][0] = cosA + temp[0] * axis[0];
    rotMat[0][1] = temp[0] * axis[1] + sinA * axis[2];
    rotMat[0][2] = temp[0] * axis[2] - sinA * axis[1];
    rotMat[1][0] = temp[1] * axis[0] - sinA * axis[2];
    rotMat[1][1] = cosA + temp[1] * axis[1];
    rotMat[1][2] = temp[1] * axis[2] + sinA * axis[0];
    rotMat[2][0] = temp[2] * axis[0] + sinA * axis[1];
    rotMat[2][1] = temp[2] * axis[1] - sinA * axis[0];
    rotMat[2][2] = cosA + temp[2] * axis[2];

    Mat4 resMat;
    resMat[0] = tm[0] * rotMat[0][0] + tm[1] * rotMat[0][1] + tm[2] * rotMat[0][2];
    resMat[1] = tm[0] * rotMat[1][0] + tm[1] * rotMat[1][1] + tm[2] * rotMat[1][2];
    resMat[2] = tm[0] * rotMat[2][0] + tm[1] * rotMat[2][1] + tm[2] * rotMat[2][2];
    resMat[3] = tm[3];

    tm = resMat;
}

void Scale(Mat4& tm, const Vec3& scale) {
    tm[0] = tm[0] * scale[0];
    tm[1] = tm[1] * scale[1];
    tm[2] = tm[2] * scale[2];
}

} // namespace Math