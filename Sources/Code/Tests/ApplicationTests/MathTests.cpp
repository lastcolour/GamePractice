#include "MathTests.hpp"
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"
#include "Math/Transform.hpp"
#include "Math/AABB.hpp"

TEST_F(MathTests, Vec2CheckConstructor) {
    Vec2 v1(1.f);
    ASSERT_EQ(v1.x, 1.f);
    ASSERT_EQ(v1.y, 1.f);

    Vec2 v2(1, 2);
    ASSERT_EQ(v2.x, 1.f);
    ASSERT_EQ(v2.y, 2.f);

    Vec2 v3;
    Vec2 v4(1.f);
    v3 = v4;
    ASSERT_EQ(v3.x, 1.f);
    ASSERT_EQ(v3.y, 1.f);

    Vec2 v5(1.f);
    Vec2 v6(v5);
    ASSERT_EQ(v6.x, 1.f);
    ASSERT_EQ(v6.y, 1.f);
}

TEST_F(MathTests, Vec2CheckOperators) {
    Vec2 v1(1.f);
    v1[0] = 2.f;
    v1[1] = 3.f;
    ASSERT_EQ(v1, Vec2(2.f, 3.f));

    Vec2 v2(1.f, 2.f);
    Vec2 v3 = v2 * 2.f;
    ASSERT_EQ(v3, Vec2(2.f, 4.f));

    Vec2 v4(1.f, 2.f);
    Vec2 v5(2.f, 3.f);
    Vec2 v6 = v4 + v5;
    ASSERT_EQ(v6, Vec2(3.f, 5.f));

    Vec2 v7(1.f, 2.f);
    v7 = v7 * 2.f;
    ASSERT_EQ(v7, Vec2(2.f, 4.f));
    Vec2 v8 = 2.f * v7;
    ASSERT_EQ(v8, Vec2(4.f, 8.f));

    Vec2 v9(1.f, 2.f);
    Vec2 v10(2.f, 4.f);
    Vec2 v11 = v9 - v10;
    ASSERT_EQ(v11, Vec2(-1.f, -2.f));

    Vec2 v12(1.f, 2.f);
    Vec2 v13(2.f, 8.f);
    Vec2 v14 = v12 / v13;
    ASSERT_EQ(v14, Vec2(0.5f, 0.25f));

    Vec2 v15(2.f, 4.f);
    v15 = v15 / 2.f;
    ASSERT_EQ(v15, Vec2(1.f, 2.f));

    Vec2 v16(1.f, 2.f);
    Vec2 v17(2.f, 3.f);
    ASSERT_TRUE(v16 < v17);
    ASSERT_FALSE(v16 > v17);
}

TEST_F(MathTests, Vec2CheckMethods) {
    Vec2 v1(1.f, 2.f);
    ASSERT_FLOAT_EQ(v1.getLenghtSq(), 5.f);

    Vec2 v2(1.f, 2.f);
    ASSERT_FLOAT_EQ(v2.getLenght(), 2.236068f);

    Vec2 v3(1.f, 2.f);
    Vec2 v4 = v3.getNormilized();
    ASSERT_EQ(v4, Vec2(0.447213590f, 0.894427180f));
}

TEST_F(MathTests, Vec3CheckConstructor) {
    Vec3 v1(1.f);
    ASSERT_FLOAT_EQ(v1.x, 1.f);
    ASSERT_FLOAT_EQ(v1.y, 1.f);
    ASSERT_FLOAT_EQ(v1.z, 1.f);

    Vec3 v2(1.f, 2.f, 3.f);
    ASSERT_FLOAT_EQ(v2.x, 1.f);
    ASSERT_FLOAT_EQ(v2.y, 2.f);
    ASSERT_FLOAT_EQ(v2.z, 3.f);

    Vec3 v3;
    Vec3 v4(1.f);
    v3 = v4;
    ASSERT_FLOAT_EQ(v3.x, 1.f);
    ASSERT_FLOAT_EQ(v3.y, 1.f);
    ASSERT_FLOAT_EQ(v3.z, 1.f);

    Vec3 v5(1.f);
    Vec3 v6(v5);
    ASSERT_FLOAT_EQ(v6.x, 1.f);
    ASSERT_FLOAT_EQ(v6.y, 1.f);
    ASSERT_FLOAT_EQ(v6.z, 1.f);
}

TEST_F(MathTests, Vec3CheckOperators) {
    Vec3 v1(1.f);
    v1[0] = 2.f;
    v1[1] = 3.f;
    v1[2] = 4.f;
    ASSERT_EQ(v1, Vec3(2.f, 3.f, 4.f));

    Vec3 v2(1.f, 2.f, 3.f);
    Vec3 v3 = v2 * 2.f;
    ASSERT_EQ(v3, Vec3(2.f, 4.f, 6.f));

    Vec3 v4(1.f, 2.f, 3.f);
    Vec3 v5(4.f, 5.f, 6.f);
    Vec3 v6 = v4 + v5;
    ASSERT_EQ(v6, Vec3(5.f, 7.f, 9.f));

    Vec3 v7(1.f, 2.f, 3.f);
    v7 = v7 * 2.f;
    ASSERT_EQ(v7, Vec3(2.f, 4.f, 6.f));
    Vec3 v8 = 2.f * v7;
    ASSERT_EQ(v8, Vec3(4.f, 8.f, 12.f));

    Vec3 v9(1.f, 2.f, 3.f);
    Vec3 v10(2.f, 4.f, 6.f);
    Vec3 v11 = v9 - v10;
    ASSERT_EQ(v11, Vec3(-1.f, -2.f, -3.f));

    Vec3 v12(1.f, 2.f, 4.f);
    Vec3 v13(2.f, 8.f, 32.f);
    Vec3 v14 = v12 / v13;
    ASSERT_EQ(v14, Vec3(0.5f, 0.25f, 0.125f));

    Vec3 v15(2.f, 4.f, 6.f);
    v15 = v15 / 2.f;
    ASSERT_EQ(v15, Vec3(1.f, 2.f, 3.f));

    Vec3 v16(1.f, 2.f, 3.f);
    Vec3 v17(4.f, 5.f, 6.f);
    ASSERT_TRUE(v16 < v17);
    ASSERT_FALSE(v16 > v17);
}

TEST_F(MathTests, Vec3CheckMethods) {
    Vec3 v1(1.f, 2.f, 3.f);
    ASSERT_FLOAT_EQ(v1.getLenghtSq(), 14.f);

    Vec3 v2(1.f, 2.f, 3.f);
    ASSERT_FLOAT_EQ(v2.getLenght(), 3.7416575f);

    Vec3 v3(1.f, 2.f, 3.f);
    Vec3 v4 = v3.getNormilized();
    ASSERT_EQ(v4, Vec3(0.267261237f, 0.534522474f, 0.801783681f));
}

TEST_F(MathTests, Vec4CheckConstructor) {
    Vec4 v1(1.f);
    ASSERT_FLOAT_EQ(v1.x, 1.f);
    ASSERT_FLOAT_EQ(v1.y, 1.f);
    ASSERT_FLOAT_EQ(v1.z, 1.f);

    Vec4 v2(1.f, 2.f, 3.f, 4.f);
    ASSERT_FLOAT_EQ(v2.x, 1.f);
    ASSERT_FLOAT_EQ(v2.y, 2.f);
    ASSERT_FLOAT_EQ(v2.z, 3.f);
    ASSERT_FLOAT_EQ(v2.w, 4.f);

    Vec4 v3;
    Vec4 v4(1.f);
    v3 = v4;
    ASSERT_FLOAT_EQ(v3.x, 1.f);
    ASSERT_FLOAT_EQ(v3.y, 1.f);
    ASSERT_FLOAT_EQ(v3.z, 1.f);
    ASSERT_FLOAT_EQ(v3.w, 1.f);

    Vec4 v5(1.f);
    Vec4 v6(v5);
    ASSERT_FLOAT_EQ(v6.x, 1.f);
    ASSERT_FLOAT_EQ(v6.y, 1.f);
    ASSERT_FLOAT_EQ(v6.z, 1.f);
    ASSERT_FLOAT_EQ(v6.w, 1.f);
}

TEST_F(MathTests, Vec4CheckOperators) {
    Vec4 v1(1.f);
    v1[0] = 2.f;
    v1[1] = 3.f;
    v1[2] = 4.f;
    v1[3] = 5.f;
    ASSERT_EQ(v1, Vec4(2.f, 3.f, 4.f, 5.f));

    Vec4 v2(1.f, 2.f, 3.f, 4.f);
    Vec4 v3 = v2 * 2.f;
    ASSERT_EQ(v3, Vec4(2.f, 4.f, 6.f, 8.f));

    Vec4 v4(1.f, 2.f, 3.f, 4.f);
    Vec4 v5(5.f, 6.f, 7.f, 8.f);
    Vec4 v6 = v4 + v5;
    ASSERT_EQ(v6, Vec4(6.f, 8.f, 10.f, 12.f));

    Vec4 v7(1.f, 2.f, 3.f, 4.f);
    v7 = v7 * 2.f;
    ASSERT_EQ(v7, Vec4(2.f, 4.f, 6.f, 8.f));
    Vec4 v8 = 2.f * v7;
    ASSERT_EQ(v8, Vec4(4.f, 8.f, 12.f, 16.f));

    Vec4 v9(1.f, 2.f, 3.f, 4.f);
    Vec4 v10(2.f, 4.f, 6.f, 8.f);
    Vec4 v11 = v9 - v10;
    ASSERT_EQ(v11, Vec4(-1.f, -2.f, -3.f, -4.f));

    Vec4 v12(1.f, 2.f, 4.f, 8.f);
    Vec4 v13(2.f, 8.f, 32.f, 128.f);
    Vec4 v14 = v12 / v13;
    ASSERT_EQ(v14, Vec4(0.5f, 0.25f, 0.125f, 0.0625f));

    Vec4 v15(2.f, 4.f, 6.f, 8.f);
    v15 = v15 / 2.f;
    ASSERT_EQ(v15, Vec4(1.f, 2.f, 3.f, 4.f));

    Vec4 v16(1.f, 2.f, 3.f, 4.f);
    Vec4 v17(5.f, 6.f, 7.f, 8.f);
    ASSERT_TRUE(v16 < v17);
    ASSERT_FALSE(v16 > v17);
}

TEST_F(MathTests, Vec4CheckMethods) {
    Vec4 v1(1.f, 2.f, 3.f, 4.f);
    ASSERT_FLOAT_EQ(v1.getLenghtSq(), 30.f);

    Vec4 v2(1.f, 2.f, 3.f, 4.f);
    ASSERT_FLOAT_EQ(v2.getLenght(), 5.4772258f);

    Vec4 v3(1.f, 2.f, 3.f, 4.f);
    Vec4 v4 = v3.getNormilized();
    ASSERT_EQ(v4, Vec4(0.182574183f, 0.365148365f, 0.547722518f, 0.730296731f));
}

TEST_F(MathTests, Mat4CheckConstructor) {
    Mat4 m1(1.f);
    ASSERT_EQ(m1[0], Vec4(1.0, 0.f, 0.f, 0.f));
    ASSERT_EQ(m1[1], Vec4(0.0, 1.f, 0.f, 0.f));
    ASSERT_EQ(m1[2], Vec4(0.0, 0.f, 1.f, 0.f));
    ASSERT_EQ(m1[3], Vec4(0.0, 0.f, 0.f, 1.f));

    Vec4 col0(1.f, 2.f, 3.f, 4.f);
    Vec4 col1(5.f, 6.f, 7.f, 8.f);
    Vec4 col2(9.f, 10.f, 11.f, 12.f);
    Vec4 col3(13.f, 14.f, 15.f, 16.f);
    Mat4 m2(col0, col1, col2, col3);
    ASSERT_EQ(m2[0], col0);
    ASSERT_EQ(m2[1], col1);
    ASSERT_EQ(m2[2], col2);
    ASSERT_EQ(m2[3], col3);

    Mat4 m3;
    Mat4 m4(2.f);
    m3 = m4;
    ASSERT_EQ(m3[0], Vec4(2.0, 0.f, 0.f, 0.f));
    ASSERT_EQ(m3[1], Vec4(0.0, 2.f, 0.f, 0.f));
    ASSERT_EQ(m3[2], Vec4(0.0, 0.f, 2.f, 0.f));
    ASSERT_EQ(m3[3], Vec4(0.0, 0.f, 0.f, 2.f));

    Mat4 m5(3.f);
    Mat4 m6(m5);
    ASSERT_EQ(m6[0], Vec4(3.0, 0.f, 0.f, 0.f));
    ASSERT_EQ(m6[1], Vec4(0.0, 3.f, 0.f, 0.f));
    ASSERT_EQ(m6[2], Vec4(0.0, 0.f, 3.f, 0.f));
    ASSERT_EQ(m6[3], Vec4(0.0, 0.f, 0.f, 3.f));
}

TEST_F(MathTests, Mat4CheckOperators) {
    Mat4 m1(1.f);

    Vec4 col0(1.f, 2.f, 3.f, 4.f);
    Vec4 col1(5.f, 6.f, 7.f, 8.f);
    Vec4 col2(9.f, 10.f, 11.f, 12.f);
    Vec4 col3(13.f, 14.f, 15.f, 16.f);

    m1[0] = col0;
    m1[1] = col1;
    m1[2] = col2;
    m1[3] = col3;

    ASSERT_EQ(m1[0], col0);
    ASSERT_EQ(m1[1], col1);
    ASSERT_EQ(m1[2], col2);
    ASSERT_EQ(m1[3], col3);

    Mat4 m2(2.f);
    Vec4 v1(1.f);
    Vec4 v2 = m2 * v1;
    ASSERT_EQ(v2, Vec4(2.f));

    Mat4 m3(Vec4(1.f), Vec4(1.f), Vec4(1.f), Vec4(1.f));
    m3 = m3 * m3;
    ASSERT_EQ(m3[0], Vec4(4.f));
    ASSERT_EQ(m3[1], Vec4(4.f));
    ASSERT_EQ(m3[2], Vec4(4.f));
    ASSERT_EQ(m3[3], Vec4(4.f));
}

TEST_F(MathTests, CheckProjectOrto) {
    const int width = 64;
    const int height = 64;
    const float xShift = height * 0.5f;
    const float yShift = width * 0.5f;
    const float xCenter = width / 2.f;
    const float yCenter = height / 2.f;

    const float resXShift = 2.f * xShift / width;
    const float resYShift = 2.f * yShift / height;

    Vec4 ptCenter(xCenter, yCenter, 0.f, 1.f);
    Vec4 ptLeft(xCenter - xShift, yCenter, 0.f, 1.f);
    Vec4 ptRight(xCenter + xShift, yCenter, 0.f, 1.f);
    Vec4 ptTop(xCenter, yCenter + yShift, 0.f, 1.f);
    Vec4 ptBot(xCenter, yCenter - yShift, 0.f, 1.f);

    Mat4 m1;
    Math::ProjectOrtho(m1, 0.f, width, 0.f, height, 0.1f, 100.f);

    Vec4 resCetner = m1 * ptCenter;
    ASSERT_FLOAT_EQ(resCetner.x, 0.f);
    ASSERT_FLOAT_EQ(resCetner.y, 0.f);

    Vec4 resLeft = m1 * ptLeft;
    ASSERT_FLOAT_EQ(resLeft.x, -resXShift);
    ASSERT_FLOAT_EQ(resLeft.y, 0.f);

    Vec4 resRight = m1 * ptRight;
    ASSERT_FLOAT_EQ(resRight.x, resXShift);
    ASSERT_FLOAT_EQ(resRight.y, 0.f);

    Vec4 resTop = m1 * ptTop;
    ASSERT_FLOAT_EQ(resTop.x, 0.f);
    ASSERT_FLOAT_EQ(resTop.y, resYShift);

    Vec4 resBot = m1 * ptBot;
    ASSERT_FLOAT_EQ(resBot.x, 0.f);
    ASSERT_FLOAT_EQ(resBot.y, -resYShift);
}

TEST_F(MathTests, CheckTranslate) {
    Mat4 m(1.f);
    Vec3 pos(1.f, 2.f, 3.f);
    Math::Translate(m, pos);

    Vec4 testPos(1.f);
    Vec4 resPos = m * testPos;
    Vec4 expectedPos = testPos + Vec4(pos, 0.f);

    ASSERT_EQ(resPos, expectedPos);
}

TEST_F(MathTests, CheckRotate) {
    Mat4 m(1.f);
    Vec3 axis(0.f, 0.f, 1.f);
    float angle = 90.f;
    Math::Rotate(m, angle, axis);

    Vec4 testPos(1.f, 0.f, 0.f, 0.f);
    Vec4 resPos = m * testPos;

    ASSERT_EQ(resPos, Vec4(0.f, 1.f, 0.f, 0.f));
}

TEST_F(MathTests, CheckSlace) {
    Mat4 m(1.f);
    Vec3 scale(2.f);
    Math::Scale(m, scale);

    Vec4 testPos(1.f, 2.f, 3.f, 0.f);
    Vec4 resPos = m * testPos;

    ASSERT_EQ(resPos, Vec4(2.f, 4.f, 6.f, 0.f));
}

TEST_F(MathTests, AABBCheckConstructor) {
    AABB aabb1(Vec3(0.f), Vec3(1.f));
    ASSERT_EQ(aabb1.bot, Vec3(0.f));
    ASSERT_EQ(aabb1.top, Vec3(1.f));

    AABB aabb2;
    AABB aabb3(Vec3(0.f), Vec3(1.f));
    aabb2 = aabb3;
    ASSERT_EQ(aabb2.bot, Vec3(0.f));
    ASSERT_EQ(aabb2.top, Vec3(1.f));

    AABB aabb4(Vec3(0.f), Vec3(1.f));
    AABB aabb5(aabb4);
    ASSERT_EQ(aabb5.bot, Vec3(0.f));
    ASSERT_EQ(aabb5.top, Vec3(1.f));
}

TEST_F(MathTests, AABBCheckOperators) {
    AABB aabb;
    aabb.bot = Vec3(0.f);
    aabb.top = Vec3(2.f);

    Vec3 center = aabb.getCenter();
    ASSERT_EQ(center, Vec3(1.f));

    Vec3 size = aabb.getSize();
    ASSERT_EQ(size, Vec3(2.f));

    aabb = aabb + Vec3(2.f);
    ASSERT_EQ(aabb.getCenter(), Vec3(3.f));
    ASSERT_EQ(aabb.getSize(), Vec3(2.f));

    AABB aabb1(Vec3(0.f), Vec3(2.f));
    aabb1.setCenter(Vec3(2.f));
    ASSERT_EQ(aabb1.bot, Vec3(1.f));
    ASSERT_EQ(aabb1.top, Vec3(3.f));
}