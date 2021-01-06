#ifndef __MATRIX_TRANSFORM_HPP__
#define __MATRIX_TRANSFORM_HPP__

#include "Math/Vector.hpp"

namespace Math {

class Matrix4;
class Matrix3;
class Quaternion;

void ProjectOrtho(Matrix4& mat, float left, float right, float bot, float top, float zNear, float zFar);

void AddTranslate(Matrix4& mat, const Vec3& pos);

void AddRotate(Matrix4& mat, const Vec3& axis, float angle);

void AddRotate(Matrix4& mat, const Quaternion& quat);

void AddScale(Matrix4& mat, const Vec3& scale);

void AddTranslate2D(Matrix3& mat, const Vec2& pos);

void AddRotate2D(Matrix3& mat, float angle);

void AddScale2D(Matrix3& mat, const Vec2& scale);

Vec2 RotateVec2D(const Vec2& v, float angle);

} // namespace Math

#endif /* __MATRIX_TRANSFORM_HPP__ */