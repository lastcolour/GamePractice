#ifndef __MATRIX_TRANSFORM_HPP__
#define __MATRIX_TRANSFORM_HPP__

#include "Math/Matrix.hpp"
#include "Math/Quaternion.hpp"

namespace Math {

    void ProjectOrtho(Mat4& tm, float left, float right, float bot, float top, float zNear, float zFar);

    void Translate(Mat4& tm, const Vec3& pos);

    void Rotate(Mat4& tm, float angle, const Vec3& rot);

    void Rotate(Mat4& tm, const Quat& quat);

    void Scale(Mat4& tm, const Vec3& scale);

} // namespace Math

#endif /* __MATRIX_TRANSFORM_HPP__ */