#include "Math/Matrix.hpp"

#include <cassert>

namespace Math {

Matrix4::Matrix4() {
}

Matrix4::Matrix4(float val) {
    data[0] = Vec4(val, 0.f, 0.f, 0.f);
    data[1] = Vec4(0.f, val, 0.f, 0.f);
    data[2] = Vec4(0.f, 0.f, val, 0.f);
    data[3] = Vec4(0.f, 0.f, 0.f, val);
}

Matrix4::Matrix4(const Vec4& col0, const Vec4& col1, const Vec4& col2, const Vec4& col3) {
    data[0] = col0;
    data[1] = col1;
    data[2] = col2;
    data[3] = col3;
}

Matrix4::Matrix4(const Matrix4& mat) {
    data[0] = mat.data[0];
    data[1] = mat.data[1];
    data[2] = mat.data[2];
    data[3] = mat.data[3];
}

Matrix4& Matrix4::operator=(const Matrix4& mat) {
    if(this == &mat) {
        return *this;
    }
    data[0] = mat.data[0];
    data[1] = mat.data[1];
    data[2] = mat.data[2];
    data[3] = mat.data[3];
    return *this;
}

Vec4& Matrix4::operator[](size_t i) {
    assert( (i >= 0 && i <= 3) && "Access to wrong element");
    return data[i];
}

const Vec4& Matrix4::operator[](size_t i) const {
    assert( (i >= 0 && i <= 3) && "Access to wrong element");
    return data[i];
}

const float* Matrix4::getPtr() const {
    return data[0].getPtr();
}

Vec4 operator*(const Matrix4& m, const Vec4& v) {
    return Vec4(
        m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3],
        m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3],
        m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3],
        m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3]
    );
}

Matrix4 operator*(const Matrix4& m1, const Matrix4& m2) {
    const auto& ca0 = m1[0];
    const auto& ca1 = m1[1];
    const auto& ca2 = m1[2];
    const auto& ca3 = m1[3];

    const auto& cb0 = m2[0];
    const auto& cb1 = m2[1];
    const auto& cb2 = m2[2];
    const auto& cb3 = m2[3];

    const auto c0 = ca0 * cb0[0] + ca1 * cb0[1] + ca2 * cb0[2] + ca3 * cb0[3];
    const auto c1 = ca0 * cb1[0] + ca1 * cb1[1] + ca2 * cb1[2] + ca3 * cb1[3];
    const auto c2 = ca0 * cb2[0] + ca1 * cb2[1] + ca2 * cb2[2] + ca3 * cb2[3];
    const auto c3 = ca0 * cb3[0] + ca1 * cb3[1] + ca2 * cb3[2] + ca3 * cb3[3];

    return Matrix4(c0, c1, c2, c3);
}

} // namespace Math