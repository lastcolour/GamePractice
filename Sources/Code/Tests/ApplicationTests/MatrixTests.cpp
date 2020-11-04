#include "Math/Matrix.hpp"
#include "MatrixTests.hpp"

TEST_F(MatrixTests, Mat2_CheckContstructors) {
    Vec2 col0(1.f, 2.f);
    Vec2 col1(3.f, 4.f);

    {
        Mat2 m(col0, col1);

        EXPECT_EQ(m[0], col0);
        EXPECT_EQ(m[1], col1);
    }
    {
        Mat2 m1;
        Mat2 m2(col0, col1);

        m1 = m2;

        EXPECT_EQ(m1[0], col0);
        EXPECT_EQ(m1[1], col1);
    }
    {
        Mat2 m1(col0, col1);
        Mat2 m2(m1);

        EXPECT_EQ(m2[0], col0);
        EXPECT_EQ(m2[1], col1);
    }
}

TEST_F(MatrixTests, Mat2_CheckOperators) {
    {
        Mat2 m(2.f);
        Vec2 p(1.f);

        auto resP = m * p;

        EXPECT_FLOAT_EQ(resP.x, 2.f);
        EXPECT_FLOAT_EQ(resP.y, 2.f);
    }

    Vec2 col0(1.f, 2.f);
    Vec2 col1(3.f, 4.f);

    {
        Mat2 m1(col0 + col1, col1 + col0);
        Mat2 m2(-col1, -col0);

        Mat2 resM = m1 + m2;

        EXPECT_EQ(resM[0], col0);
        EXPECT_EQ(resM[1], col1);
    }
    {
        Mat2 m1(col0 + col1, col1 + col0);
        Mat2 m2(-col1, -col0);

        m1 += m2;

        EXPECT_EQ(m1[0], col0);
        EXPECT_EQ(m1[1], col1);
    }

    {
        Mat2 m1(col0 - col1, col1 - col0);
        Mat2 m2(-col1, -col0);

        Mat2 resM = m1 - m2;

        EXPECT_EQ(resM[0], col0);
        EXPECT_EQ(resM[1], col1);
    }
    {
        Mat2 m1(col0 - col1, col1 - col0);
        Mat2 m2(-col1, -col0);

        m1 -= m2;

        EXPECT_EQ(m1[0], col0);
        EXPECT_EQ(m1[1], col1);
    }
}

TEST_F(MatrixTests, Mat3x2_CheckContstructors) {
    Vec2 col0(1.f, 2.f);
    Vec2 col1(3.f, 4.f);
    Vec2 col2(5.f, 6.f);

    {
        Mat3x2 m(col0, col1, col2);

        EXPECT_EQ(m[0], col0);
        EXPECT_EQ(m[1], col1);
        EXPECT_EQ(m[2], col2);
    }
    {
        Mat3x2 m1;
        Mat3x2 m2(col0, col1, col2);

        m1 = m2;

        EXPECT_EQ(m1[0], col0);
        EXPECT_EQ(m1[1], col1);
        EXPECT_EQ(m1[2], col2);
    }
    {
        Mat3x2 m1(col0, col1, col2);
        Mat3x2 m2(m1);

        EXPECT_EQ(m2[0], col0);
        EXPECT_EQ(m2[1], col1);
        EXPECT_EQ(m2[2], col2);
    }
}

TEST_F(MatrixTests, Mat3x2_CheckOperators) {
    Vec2 col0(1.f, 2.f);
    Vec2 col1(3.f, 4.f);
    Vec2 col2(5.f, 6.f);

    {
        Mat3x2 m1(col0 + col1, col1 + col2, col2 + col0);
        Mat3x2 m2(-col1, -col2, -col0);

        Mat3x2 resM = m1 + m2;

        EXPECT_EQ(resM[0], col0);
        EXPECT_EQ(resM[1], col1);
        EXPECT_EQ(resM[2], col2);
    }
    {
        Mat3x2 m1(col0 + col1, col1 + col2, col2 + col0);
        Mat3x2 m2(-col1, -col2, -col0);

        m1 += m2;

        EXPECT_EQ(m1[0], col0);
        EXPECT_EQ(m1[1], col1);
        EXPECT_EQ(m1[2], col2);
    }

    {
        Mat3x2 m1(col0 - col1, col1 - col2, col2 - col0);
        Mat3x2 m2(-col1, -col2, -col0);

        Mat3x2 resM = m1 - m2;

        EXPECT_EQ(resM[0], col0);
        EXPECT_EQ(resM[1], col1);
        EXPECT_EQ(resM[2], col2);
    }
    {
        Mat3x2 m1(col0 - col1, col1 - col2, col2 - col0);
        Mat3x2 m2(-col1, -col2, -col0);

        m1 -= m2;

        EXPECT_EQ(m1[0], col0);
        EXPECT_EQ(m1[1], col1);
        EXPECT_EQ(m1[2], col2);
    }

    {
        Mat3x2 m1(col0, col1, col2);
        Mat3 m2 = m1.toMat3();

        EXPECT_EQ(m2[0], Vec3(col0.x, col0.y, 0.f));
        EXPECT_EQ(m2[1], Vec3(col1.x, col1.y, 0.f));
        EXPECT_EQ(m2[2], Vec3(col2.x, col2.y, 1.f));
    }
}

TEST_F(MatrixTests, Mat3_CheckContstructors) {
    {
        Mat3 m(2.f);
        EXPECT_EQ(m[0], Vec3(2.f, 0.f, 0.f));
        EXPECT_EQ(m[1], Vec3(0.f, 2.f, 0.f));
        EXPECT_EQ(m[2], Vec3(0.f, 0.f, 2.f));
    }
    {
        Vec3 col0(1.f, 2.f, 3.f);
        Vec3 col1(4.f, 5.f, 6.f);
        Vec3 col2(7.f, 8.f, 9.f);

        Mat3 m(col0, col1, col2);

        EXPECT_EQ(m[0], col0);
        EXPECT_EQ(m[1], col1);
        EXPECT_EQ(m[2], col2);
    }
    {
        Mat3 m1;
        Mat3 m2(2.f);

        m1 = m2;

        EXPECT_EQ(m1[0], Vec3(2.0, 0.f, 0.f));
        EXPECT_EQ(m1[1], Vec3(0.0, 2.f, 0.f));
        EXPECT_EQ(m1[2], Vec3(0.0, 0.f, 2.f));
    }
    {
        Mat3 m1;
        Mat3 m2(2.f);

        m1 = m2;

        EXPECT_EQ(m1[0], Vec3(2.0, 0.f, 0.f));
        EXPECT_EQ(m1[1], Vec3(0.0, 2.f, 0.f));
        EXPECT_EQ(m1[2], Vec3(0.0, 0.f, 2.f));
    }
    {
        Mat3 m1(3.f);
        Mat3 m2(m1);

        EXPECT_EQ(m2[0], Vec3(3.0, 0.f, 0.f));
        EXPECT_EQ(m2[1], Vec3(0.0, 3.f, 0.f));
        EXPECT_EQ(m2[2], Vec3(0.0, 0.f, 3.f));
    }
}

TEST_F(MatrixTests, Mat3_CheckOperators) {
    {
        Mat3 m(2.f);
        Vec3 p(1.f);

        auto resP = m * p;

        EXPECT_FLOAT_EQ(resP.x, 2.f);
        EXPECT_FLOAT_EQ(resP.y, 2.f);
        EXPECT_FLOAT_EQ(resP.z, 2.f);
    }

    Vec3 col0(1.f, 2.f, 3.f);
    Vec3 col1(4.f, 5.f, 6.f);
    Vec3 col2(7.f, 8.f, 9.f);

    {
        Mat3 m1(col0 + col1, col1 + col2, col2 + col0);
        Mat3 m2(-col1, -col2, -col0);

        Mat3 resM = m1 + m2;

        EXPECT_EQ(resM[0], col0);
        EXPECT_EQ(resM[1], col1);
        EXPECT_EQ(resM[2], col2);
    }
    {
        Mat3 m1(col0 + col1, col1 + col2, col2 + col0);
        Mat3 m2(-col1, -col2, -col0);

        m1 += m2;

        EXPECT_EQ(m1[0], col0);
        EXPECT_EQ(m1[1], col1);
        EXPECT_EQ(m1[2], col2);
    }

    {
        Mat3 m1(col0 - col1, col1 - col2, col2 - col0);
        Mat3 m2(-col1, -col2, -col0);

        Mat3 resM = m1 - m2;

        EXPECT_EQ(resM[0], col0);
        EXPECT_EQ(resM[1], col1);
        EXPECT_EQ(resM[2], col2);
    }
    {
        Mat3 m1(col0 - col1, col1 - col2, col2 - col0);
        Mat3 m2(-col1, -col2, -col0);

        m1 -= m2;

        EXPECT_EQ(m1[0], col0);
        EXPECT_EQ(m1[1], col1);
        EXPECT_EQ(m1[2], col2);
    }

    {
        Mat3 m1(2.f);
        Mat3 m2(col0, col1, col2);

        Mat3 resM = m1 * m2;

        EXPECT_EQ(resM[0], 2.f * col0);
        EXPECT_EQ(resM[1], 2.f * col1);
        EXPECT_EQ(resM[2], 2.f * col2);
    }
    {
        Mat3 m1(2.f);
        Mat3 m2(col0, col1, col2);

        Mat3 resM = m1;
        resM *= m2;

        EXPECT_EQ(resM[0], 2.f * col0);
        EXPECT_EQ(resM[1], 2.f * col1);
        EXPECT_EQ(resM[2], 2.f * col2);
    }

    {
        Mat3 m1(col0, col1, col2);
        Mat3x2 m2 = m1.toMat3x2();

        EXPECT_EQ(m2[0], Vec2(col0.x, col0.y));
        EXPECT_EQ(m2[1], Vec2(col1.x, col1.y));
        EXPECT_EQ(m2[2], Vec2(col2.x, col2.y));
    }
}

TEST_F(MatrixTests, Mat4x3_CheckContstructors) {
    Vec3 col0(1.f, 2.f, 3.f);
    Vec3 col1(4.f, 5.f, 6.f);
    Vec3 col2(7.f, 8.f, 9.f);
    Vec3 col3(10.f, 11.f, 12.f);

    {
        Mat4x3 m(col0, col1, col2, col3);

        EXPECT_EQ(m[0], col0);
        EXPECT_EQ(m[1], col1);
        EXPECT_EQ(m[2], col2);
    }
    {
        Mat4x3 m1;
        Mat4x3 m2(col0, col1, col2, col3);

        m1 = m2;

        EXPECT_EQ(m1[0], col0);
        EXPECT_EQ(m1[1], col1);
        EXPECT_EQ(m1[2], col2);
        EXPECT_EQ(m1[3], col3);
    }
    {
        Mat4x3 m1(col0, col1, col2, col3);
        Mat4x3 m2(m1);

        EXPECT_EQ(m2[0], col0);
        EXPECT_EQ(m2[1], col1);
        EXPECT_EQ(m2[2], col2);
        EXPECT_EQ(m2[3], col3);
    }
}

TEST_F(MatrixTests, Mat4x3_CheckOperators) {
    Vec3 col0(1.f, 2.f, 3.f);
    Vec3 col1(4.f, 5.f, 6.f);
    Vec3 col2(7.f, 8.f, 9.f);
    Vec3 col3(10.f, 11.f, 12.f);

    {
        Mat4x3 m1(col0 + col1, col1 + col2, col2 + col3, col3 + col0);
        Mat4x3 m2(-col1, -col2, -col3, -col0);

        Mat4x3 resM = m1 + m2;

        EXPECT_EQ(resM[0], col0);
        EXPECT_EQ(resM[1], col1);
        EXPECT_EQ(resM[2], col2);
        EXPECT_EQ(resM[3], col3);
    }
    {
        Mat4x3 m1(col0 + col1, col1 + col2, col2 + col3, col3 + col0);
        Mat4x3 m2(-col1, -col2, -col3, -col0);

        m1 += m2;

        EXPECT_EQ(m1[0], col0);
        EXPECT_EQ(m1[1], col1);
        EXPECT_EQ(m1[2], col2);
        EXPECT_EQ(m1[3], col3);
    }

    {
        Mat4x3 m1(col0 - col1, col1 - col2, col2 - col3, col3 - col0);
        Mat4x3 m2(-col1, -col2, -col3, -col0);

        Mat4x3 resM = m1 - m2;

        EXPECT_EQ(resM[0], col0);
        EXPECT_EQ(resM[1], col1);
        EXPECT_EQ(resM[2], col2);
        EXPECT_EQ(resM[3], col3);
    }
    {
        Mat4x3 m1(col0 - col1, col1 - col2, col2 - col3, col3 - col0);
        Mat4x3 m2(-col1, -col2, -col3, -col0);

        m1 -= m2;

        EXPECT_EQ(m1[0], col0);
        EXPECT_EQ(m1[1], col1);
        EXPECT_EQ(m1[2], col2);
        EXPECT_EQ(m1[3], col3);
    }

    {
        Mat4x3 m1(col0, col1, col2, col3);
        Mat4 m2 = m1.toMat4();

        EXPECT_EQ(m2[0], Vec4(col0, 0.f));
        EXPECT_EQ(m2[1], Vec4(col1, 0.f));
        EXPECT_EQ(m2[2], Vec4(col2, 0.f));
        EXPECT_EQ(m2[3], Vec4(col3, 1.f));
    }
}

TEST_F(MatrixTests, Mat4_CheckContstructors) {
    {
        Mat4 m(2.f);
        EXPECT_EQ(m[0], Vec4(2.f, 0.f, 0.f, 0.f));
        EXPECT_EQ(m[1], Vec4(0.f, 2.f, 0.f, 0.f));
        EXPECT_EQ(m[2], Vec4(0.f, 0.f, 2.f, 0.f));
        EXPECT_EQ(m[3], Vec4(0.f, 0.f, 0.f, 2.f));
    }
    {
        Vec4 col0(1.f, 2.f, 3.f, 4.f);
        Vec4 col1(5.f, 6.f, 7.f, 8.f);
        Vec4 col2(9.f, 10.f, 11.f, 12.f);
        Vec4 col3(13.f, 14.f, 15.f, 16.f);

        Mat4 m(col0, col1, col2, col3);

        EXPECT_EQ(m[0], col0);
        EXPECT_EQ(m[1], col1);
        EXPECT_EQ(m[2], col2);
        EXPECT_EQ(m[3], col3);
    }
    {
        Mat4 m1;
        Mat4 m2(2.f);

        m1 = m2;

        EXPECT_EQ(m1[0], Vec4(2.0, 0.f, 0.f, 0.f));
        EXPECT_EQ(m1[1], Vec4(0.0, 2.f, 0.f, 0.f));
        EXPECT_EQ(m1[2], Vec4(0.0, 0.f, 2.f, 0.f));
        EXPECT_EQ(m1[3], Vec4(0.0, 0.f, 0.f, 2.f));
    }
    {
        Mat4 m1;
        Mat4 m2(2.f);

        m1 = m2;

        EXPECT_EQ(m1[0], Vec4(2.0, 0.f, 0.f, 0.f));
        EXPECT_EQ(m1[1], Vec4(0.0, 2.f, 0.f, 0.f));
        EXPECT_EQ(m1[2], Vec4(0.0, 0.f, 2.f, 0.f));
        EXPECT_EQ(m1[3], Vec4(0.0, 0.f, 0.f, 2.f));
    }
    {
        Mat4 m1(3.f);
        Mat4 m2(m1);

        EXPECT_EQ(m2[0], Vec4(3.0, 0.f, 0.f, 0.f));
        EXPECT_EQ(m2[1], Vec4(0.0, 3.f, 0.f, 0.f));
        EXPECT_EQ(m2[2], Vec4(0.0, 0.f, 3.f, 0.f));
        EXPECT_EQ(m2[3], Vec4(0.0, 0.f, 0.f, 3.f));
    }
}

TEST_F(MatrixTests, Mat4_CheckOperators) {
    {
        Mat4 m(2.f);
        Vec4 p(1.f);

        auto resP = m * p;

        EXPECT_FLOAT_EQ(resP.x, 2.f);
        EXPECT_FLOAT_EQ(resP.y, 2.f);
        EXPECT_FLOAT_EQ(resP.z, 2.f);
        EXPECT_FLOAT_EQ(resP.w, 2.f);
    }

    Vec4 col0(1.f, 2.f, 3.f, 4.f);
    Vec4 col1(5.f, 6.f, 7.f, 8.f);
    Vec4 col2(9.f, 10.f, 11.f, 12.f);
    Vec4 col3(13.f, 14.f, 15.f, 16.f);

    {
        Mat4 m1(col0 + col1, col1 + col2, col2 + col3, col3 + col0);
        Mat4 m2(-col1, -col2, -col3, -col0);

        Mat4 resM = m1 + m2;

        EXPECT_EQ(resM[0], col0);
        EXPECT_EQ(resM[1], col1);
        EXPECT_EQ(resM[2], col2);
        EXPECT_EQ(resM[3], col3);
    }
    {
        Mat4 m1(col0 + col1, col1 + col2, col2 + col3, col3 + col0);
        Mat4 m2(-col1, -col2, -col3, -col0);

        m1 += m2;

        EXPECT_EQ(m1[0], col0);
        EXPECT_EQ(m1[1], col1);
        EXPECT_EQ(m1[2], col2);
        EXPECT_EQ(m1[3], col3);
    }

    {
        Mat4 m1(col0 - col1, col1 - col2, col2 - col3, col3 - col0);
        Mat4 m2(-col1, -col2, -col3, -col0);

        Mat4 resM = m1 - m2;

        EXPECT_EQ(resM[0], col0);
        EXPECT_EQ(resM[1], col1);
        EXPECT_EQ(resM[2], col2);
        EXPECT_EQ(resM[3], col3);
    }
    {
        Mat4 m1(col0 - col1, col1 - col2, col2 - col3, col3 - col0);
        Mat4 m2(-col1, -col2, -col3, -col0);

        m1 -= m2;

        EXPECT_EQ(m1[0], col0);
        EXPECT_EQ(m1[1], col1);
        EXPECT_EQ(m1[2], col2);
        EXPECT_EQ(m1[3], col3);
    }

    {
        Mat4 m1(2.f);
        Mat4 m2(col0, col1, col2, col3);

        Mat4 resM = m1 * m2;

        EXPECT_EQ(resM[0], 2.f * col0);
        EXPECT_EQ(resM[1], 2.f * col1);
        EXPECT_EQ(resM[2], 2.f * col2);
        EXPECT_EQ(resM[3], 2.f * col3);
    }
    {
        Mat4 m1(2.f);
        Mat4 m2(col0, col1, col2, col3);

        Mat4 resM = m1;
        resM *= m2;

        EXPECT_EQ(resM[0], 2.f * col0);
        EXPECT_EQ(resM[1], 2.f * col1);
        EXPECT_EQ(resM[2], 2.f * col2);
        EXPECT_EQ(resM[3], 2.f * col3);
    }

    {
        Mat4 m1(col0, col1, col2, col3);
        Mat4x3 m2 = m1.toMat4x3();

        EXPECT_EQ(m2[0], Vec3(col0.x, col0.y, col0.z));
        EXPECT_EQ(m2[1], Vec3(col1.x, col1.y, col1.z));
        EXPECT_EQ(m2[2], Vec3(col2.x, col2.y, col2.z));
        EXPECT_EQ(m2[3], Vec3(col3.x, col3.y, col3.z));
    }
}