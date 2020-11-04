#ifndef __MATRIX_TRANSFORM_HPP__
#define __MATRIX_TRANSFORM_HPP__

#include "Math/Vector.hpp"

namespace Math {

class Matrix4;
class Matrix3;
class Quaternion;

void ProjectOrtho(Matrix4& mat, float left, float right, float bot, float top, float zNear, float zFar);

void Translate(Matrix4& mat, const Vec3& pos);

void Rotate(Matrix4& mat, const Vec3& axis, float angle);

void Rotate(Matrix4& mat, const Quaternion& quat);

void Scale(Matrix4& mat, const Vec3& scale);

void Translate2D(Matrix3& mat, const Vec2& pos);

void Rotate2D(Matrix3& mat, float angle);

void Scale2D(Matrix3& mat, const Vec2& scale);

} // namespace Math

#endif /* __MATRIX_TRANSFORM_HPP__ */