#include "MathTests.hpp"
#include "Math/Vector.hpp"

TEST_F(MathTests, Vec2iCheckConstructor) {
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