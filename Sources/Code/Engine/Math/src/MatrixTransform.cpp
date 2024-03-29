#include "Math/MatrixTransform.hpp"
#include "Math/Matrix.hpp"
#include "Math/Quaternion.hpp"

namespace {

Mat4 Mat4toMat3Mult(const Mat4& ma, const Mat3& mb) {
    Mat4 resMat;

    resMat[0] = ma[0] * mb[0][0] + ma[1] * mb[0][1] + ma[2] * mb[0][2];
    resMat[1] = ma[0] * mb[1][0] + ma[1] * mb[1][1] + ma[2] * mb[1][2];
    resMat[2] = ma[0] * mb[2][0] + ma[1] * mb[2][1] + ma[2] * mb[2][2];
    resMat[3] = ma[3];

    return resMat;
}

Mat3 Mat3toMat2Mult(const Mat3& ma, const Mat2& mb) {
    Mat3 resMat;

    resMat[0] = ma[0] * mb[0][0] + ma[1] * mb[0][1];
    resMat[1] = ma[0] * mb[1][0] + ma[1] * mb[1][1];
    resMat[2] = ma[2];

    return resMat;
}

} // namespace

namespace Math {

void ProjectOrtho(Mat4& mat, float left, float right, float bot, float top, float zNear, float zFar) {
    mat = Mat4(1.f);
    mat[0][0] = 2.f / (right - left);
    mat[1][1] = 2.f / (top - bot);
    mat[2][2] = -2.f / (zFar - zNear);
    mat[3][0] = -(right + left) / (right - left);
    mat[3][1] = -(top + bot) / (top - bot);
    mat[3][2] = -(zFar + zNear) / (zFar - zNear);
}

Vec2 RotateVec2D(const Vec2& v, float angle) {
    Vec2 res;
    float cosA = cos(angle);
    float sinA = sin(angle);
    res.x = v.x * cosA - v.y * sinA;
    res.y = v.x * sinA + v.y * cosA;
    return res;
}

void AddTranslate2D(Matrix3x2& mat, const Vec2& pos) {
    mat[2] = mat[0] * pos[0] + mat[1] * pos[1] + mat[2];
}

void AddTranslate2D(Matrix3& mat, const Vec2& pos) {
    mat[2] = mat[0] * pos[0] + mat[1] * pos[1] + mat[2];
}

void AddTranslate3D(Matrix4x3& mat, const Vec3& pos) {
    mat[3] = mat[0] * pos[0] + mat[1] * pos[1] + mat[2] * pos[2] + mat[3];
}

void AddTranslate3D(Matrix4& mat, const Vec3& pos) {
    mat[3] = mat[0] * pos[0] + mat[1] * pos[1] + mat[2] * pos[2] + mat[3];
}

void AddRotate2D(Matrix3& mat, float angle) {
    float cosA = cos(angle);
    float sinA = sin(angle);

    Mat2 rotMat;
    rotMat[0][0] = cosA;
    rotMat[0][1] = sinA;
    rotMat[1][0] = -sinA;
    rotMat[1][1] = cosA;

    mat = Mat3toMat2Mult(mat, rotMat);
}

void AddRotate3D(Matrix4& mat, const Vec3& axis, float angle) {
    const float cosA = cos(angle);
    const float sinA = sin(angle);
    const Vec3 temp = (1.f - cosA) * axis;

    Mat3 rotMat;
    rotMat[0][0] = cosA + temp[0] * axis[0];
    rotMat[0][1] = temp[0] * axis[1] + sinA * axis[2];
    rotMat[0][2] = temp[0] * axis[2] - sinA * axis[1];
    rotMat[1][0] = temp[1] * axis[0] - sinA * axis[2];
    rotMat[1][1] = cosA + temp[1] * axis[1];
    rotMat[1][2] = temp[1] * axis[2] + sinA * axis[0];
    rotMat[2][0] = temp[2] * axis[0] + sinA * axis[1];
    rotMat[2][1] = temp[2] * axis[1] - sinA * axis[0];
    rotMat[2][2] = cosA + temp[2] * axis[2];

    mat = Mat4toMat3Mult(mat, rotMat);
}

void AddRotate3D(Matrix4& mat, const Quaternion& quat) {
    Mat3 rotMat = quat.toMat3();
    mat = Mat4toMat3Mult(mat, rotMat);
}

void AddScale3D(Matrix4& mat, const Vec3& scale) {
    mat[0] *= scale[0];
    mat[1] *= scale[1];
    mat[2] *= scale[2];
}

void AddScale2D(Matrix3& mat, const Vec2& scale) {
    mat[0] = Vec3(mat[0][0] * scale[0], mat[0][1] * scale[1], mat[0][2]);
    mat[1] = Vec3(mat[1][0] * scale[0], mat[1][1] * scale[1], mat[1][2]);
}

Vec2 GetScale2D(const Matrix3x2& mat) {
    Vec2 s;
    s[0] = mat[0].length();
    s[1] = mat[1].length();
    return s;
}

Vec2 GetScale2D(const Matrix3& mat) {
    Vec2 s;
    s[0] = mat[0].length();
    s[1] = mat[1].length();
    return s;
}

Vec3 GetScale3D(const Matrix4x3& mat) {
    Vec3 s;
    s[0] = mat[0].length();
    s[1] = mat[1].length();
    s[2] = mat[2].length();
    return s;
}

Vec3 GetScale3D(const Matrix4& mat) {
    Vec3 s;
    s[0] = Vec3(mat[0][0], mat[0][1], mat[0][2]).length();
    s[1] = Vec3(mat[1][0], mat[1][1], mat[1][2]).length();
    s[2] = Vec3(mat[2][0], mat[2][1], mat[2][2]).length();
    return s;
}

Transform GetTransform(const Matrix4& mat) {
    Transform tm;
    tm.scale = GetScale3D(mat);

    Matrix3 rotMat = {
        Vec3(mat[0][0], mat[0][1], mat[0][2]) / tm.scale.x,
        Vec3(mat[1][0], mat[1][1], mat[1][2]) / tm.scale.y,
        Vec3(mat[2][0], mat[2][1], mat[2][2]) / tm.scale.z};
    tm.quat.setRotationMat(rotMat);

    tm.pt = Vec3(mat[3][0], mat[3][1], mat[3][2]);
    return tm;
}

} // namespace Math