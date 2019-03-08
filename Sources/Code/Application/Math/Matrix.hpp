#ifndef __MATRIX_HPP__
#define __MATRIX_HPP__

#include "Math/Vector.hpp"

namespace Math {

class Matrix4 {
public:

    Matrix4();
    Matrix4(float val);
    Matrix4(const Vec4& col0, const Vec4& col1, const Vec4& col2, const Vec4& col3);
    Matrix4(const Matrix4& mat);
    Matrix4& operator=(const Matrix4& mat);

    Vec4& operator[](int i);
    const Vec4& operator[](int i) const;

    const float* getPtr() const;

private:

    Vec4 data[4];
};

Vec4 operator*(const Matrix4& m, const Vec4& v);
Matrix4 operator*(const Matrix4& m1, const Matrix4& m2);

} // namespace Math

typedef Math::Matrix4 Mat4;

#endif /* __MATRIX_HPP__ */