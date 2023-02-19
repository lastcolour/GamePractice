#ifndef __MATRIX_TRANSFORM_HPP__
#define __MATRIX_TRANSFORM_HPP__

#include "Math/Transform.hpp"

namespace Math {

class Matrix3x2;
class Matrix3;
class Matrix4x3;
class Matrix4;

void ProjectOrtho(Matrix4& mat, float left, float right, float bot, float top, float zNear, float zFar);

Vec2 RotateVec2D(const Vec2& v, float angle);

void AddTranslate2D(Matrix3x2& mat, const Vec2& pos);

void AddTranslate2D(Matrix3& mat, const Vec2& pos);

void AddTranslate3D(Matrix4x3& mat, const Vec3& pos);

void AddTranslate3D(Matrix4& mat, const Vec3& pos);

void AddRotate2D(Matrix3& mat, float angle);

void AddRotate3D(Matrix4& mat, const Vec3& axis, float angle);

void AddRotate3D(Matrix4& mat, const Quaternion& quat);

void AddScale3D(Matrix4& mat, const Vec3& scale);

void AddScale2D(Matrix3& mat, const Vec2& scale);

Vec2 GetScale2D(const Matrix3x2& mat);

Vec2 GetScale2D(const Matrix3& mat);

Vec3 GetScale3D(const Matrix4x3& mat);

Vec3 GetScale3D(const Matrix4& mat);

Transform GetTransform(const Matrix4x3& mat);

Transform GetTransform(const Matrix4& mat);

} // namespace Math

#endif /* __MATRIX_TRANSFORM_HPP__ */