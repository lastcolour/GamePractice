#include "MathTests.hpp"
#include "Math/Matrix.hpp"
#include "Math/MatrixTransform.hpp"
#include "Math/AABB.hpp"

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

TEST_F(MathTests, CheckTranslate2D) {
    Mat3 m(1.f);
    Vec2 pos(1.f, 2.f);
    Math::AddTranslate2D(m, pos);

    Vec3 testPos(1.f);
    Vec3 resPos = m * testPos;
    Vec3 expectedPos = testPos + Vec3(pos, 0.f);

    ASSERT_EQ(resPos, expectedPos);
}

TEST_F(MathTests, CheckRotate2D) {
    Mat3 m(1.f);
    Math::AddRotate2D(m, Math::Deg2Rad(90.f));

    Vec3 testPos(1.f, 0.f, 0.f);
    Vec3 resPos = m * testPos;

    ASSERT_EQ(resPos, Vec3(0.f, 1.f, 0.f));
}

TEST_F(MathTests, CheckScale2D) {
    Mat3 m(1.f);
    Vec2 scale(2.f);
    Math::AddScale2D(m, scale);

    Vec3 testPos(1.f, 2.f, 0.f);
    Vec3 resPos = m * testPos;

    ASSERT_EQ(resPos, Vec3(2.f, 4.f, 0.f));
}

TEST_F(MathTests, CheckTranslate) {
    Mat4 m(1.f);
    Vec3 pos(1.f, 2.f, 3.f);
    Math::AddTranslate(m, pos);

    Vec4 testPos(1.f);
    Vec4 resPos = m * testPos;
    Vec4 expectedPos = testPos + Vec4(pos, 0.f);

    ASSERT_EQ(resPos, expectedPos);
}

TEST_F(MathTests, CheckRotate) {
    Mat4 m(1.f);
    Vec3 axis(0.f, 0.f, 1.f);
    float angle = Math::Deg2Rad(90.f);
    Math::AddRotate(m, axis, angle);

    Vec4 testPos(1.f, 0.f, 0.f, 0.f);
    Vec4 resPos = m * testPos;

    ASSERT_EQ(resPos, Vec4(0.f, 1.f, 0.f, 0.f));
}

TEST_F(MathTests, CheckScale) {
    Mat4 m(1.f);
    Vec3 scale(2.f);
    Math::AddScale(m, scale);

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

TEST_F(MathTests, CheckLerpVec) {
    Vec3 start(0.f);
    Vec3 end(1.f);

    Vec3 res = Math::Lerp(start, end, 0.f);
    ASSERT_EQ(res, start);

    res = Math::Lerp(start, end, 0.2f);
    ASSERT_EQ(res, Vec3(0.2f));

    res = Math::Lerp(start, end, 0.5f);
    ASSERT_EQ(res, Vec3(0.5f));

    res = Math::Lerp(start, end, 0.7f);
    ASSERT_EQ(res, Vec3(0.7f));

    res = Math::Lerp(start, end, 1.f);
    ASSERT_EQ(res, end);
}