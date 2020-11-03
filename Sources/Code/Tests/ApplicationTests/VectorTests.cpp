#include "VectorTests.hpp"
#include "Math/Vector.hpp"

TEST_F(VectorTests, Vec2_CheckConstructors) {
    {
        Vec2 v(1.f);

        EXPECT_FLOAT_EQ(v.x, 1.f);
        EXPECT_FLOAT_EQ(v.y, 1.f);
    }

    {
        Vec2 v(1.f, 2.f);

        EXPECT_FLOAT_EQ(v.x, 1.f);
        EXPECT_FLOAT_EQ(v.y, 2.f);
    }

    {
        Vec2 v1(1.f, 2.f);
        Vec2 v2(v1);

        EXPECT_FLOAT_EQ(v2.x, v1.x);
        EXPECT_FLOAT_EQ(v2.y, v1.y);
    }

    {
        Vec2 v1(1.f, 2.f);
        Vec2 v2(0.f);
        v2 = v1;

        EXPECT_FLOAT_EQ(v2.x, v1.x);
        EXPECT_FLOAT_EQ(v2.y, v1.y);
    }
}

TEST_F(VectorTests, Vec2_CheckOperators) {
    {
        Vec2 v(1.f, 2.f);

        EXPECT_FLOAT_EQ(v[0], 1.f);
        EXPECT_FLOAT_EQ(v[1], 2.f);
    }

    {
        Vec2 v(0.f);

        v[0] = 1.f;
        v[1] = 2.f;

        EXPECT_FLOAT_EQ(v[0], 1.f);
        EXPECT_FLOAT_EQ(v[1], 2.f);
    }

    {
        Vec2 v1(1.f, 2.f);
        Vec2 v2(3.f, 4.f);

        Vec2 v3 = v1 + v2;

        EXPECT_FLOAT_EQ(v3.x, 4.f);
        EXPECT_FLOAT_EQ(v3.y, 6.f);
    }
    {
        Vec2 v1(1.f, 2.f);
        Vec2 v2(3.f, 4.f);

         v2 += v1;

        EXPECT_FLOAT_EQ(v2.x, 4.f);
        EXPECT_FLOAT_EQ(v2.y, 6.f);
    }

    {
        Vec2 v1(1.f, 2.f);
        Vec2 v2(3.f, 4.f);

        Vec2 v3 = v2 - v1;

        EXPECT_FLOAT_EQ(v3.x, 2.f);
        EXPECT_FLOAT_EQ(v3.y, 2.f);
    }
    {
        Vec2 v1(1.f, 2.f);
        Vec2 v2(3.f, 4.f);

         v2 -= v1;

        EXPECT_FLOAT_EQ(v2.x, 2.f);
        EXPECT_FLOAT_EQ(v2.y, 2.f);
    }
    {
        Vec2 v1(1.f, 2.f);
        Vec2 v2 = -v1;

        EXPECT_FLOAT_EQ(v2.x, -v1.x);
        EXPECT_FLOAT_EQ(v2.y, -v1.y);
    }

    {
        Vec2 v1(1.f, 2.f);
        Vec2 v2;

        v2 = v1 * 0.5f;

        EXPECT_FLOAT_EQ(v2.x, 0.5f);
        EXPECT_FLOAT_EQ(v2.y, 1.f);
    }
    {
        Vec2 v1(1.f, 2.f);
        Vec2 v2;

        v2 = 0.5f * v1;

        EXPECT_FLOAT_EQ(v2.x, 0.5f);
        EXPECT_FLOAT_EQ(v2.y, 1.f);
    }

    {
        Vec2 v1(1.f, 2.f);
        Vec2 v2;

        v2 = v1 / 2.f;

        EXPECT_FLOAT_EQ(v2.x, 0.5f);
        EXPECT_FLOAT_EQ(v2.y, 1.f);
    }

    {
        Vec2 v(1.f, 2.f);

        v *= 0.5f;

        EXPECT_FLOAT_EQ(v.x, 0.5f);
        EXPECT_FLOAT_EQ(v.y, 1.f);
    }
    {
        Vec2 v(1.f, 2.f);

        v /= 2.f;

        EXPECT_FLOAT_EQ(v.x, 0.5f);
        EXPECT_FLOAT_EQ(v.y, 1.f);
    }
}

TEST_F(VectorTests, Vec2_CheckMethods) {
    {
        Vec2 v(2.f);
        v.scale(Vec2(0.5, 0.25f));

        EXPECT_FLOAT_EQ(v.x, 1.f);
        EXPECT_FLOAT_EQ(v.y, 0.5f);
    }
    {
        Vec2 v(2.f);
        v.scale(0.5, 0.25f);

        EXPECT_FLOAT_EQ(v.x, 1.f);
        EXPECT_FLOAT_EQ(v.y, 0.5f);
    }

    {
        Vec2 v1(2.f);
        Vec2 v2 = v1.getScaled(Vec2(0.5, 0.25f));

        EXPECT_FLOAT_EQ(v2.x, 1.f);
        EXPECT_FLOAT_EQ(v2.y, 0.5f);
    }
    {
        Vec2 v1(2.f);
        Vec2 v2 = v1.getScaled(0.5, 0.25f);

        EXPECT_FLOAT_EQ(v2.x, 1.f);
        EXPECT_FLOAT_EQ(v2.y, 0.5f);
    }

    {
        Vec2 v(3.f, 4.f);

        EXPECT_FLOAT_EQ(v.getLenghtSq(), 25.f);
        EXPECT_FLOAT_EQ(v.getLenght(), 5.f);
    }

    {
        Vec2 v1(2.f, 2.f);

        Vec2 v2 = v1.getNormilized();

        EXPECT_FLOAT_EQ(v2.x, 0.70710677f);
        EXPECT_FLOAT_EQ(v2.y, 0.70710677f);
    }
    {
        Vec2 v(2.f, 2.f);
        v.normalize();

        EXPECT_FLOAT_EQ(v.x, 0.70710677f);
        EXPECT_FLOAT_EQ(v.y, 0.70710677f);
    }
}

TEST_F(VectorTests, Vec2_CheckCompare) {
    {
        Vec2 v1(0.f, 1.f);
        Vec2 v2(1.f, 2.f);

        EXPECT_TRUE(v1 != v2);
        EXPECT_FALSE(v1 == v2);
        EXPECT_TRUE(v1 < v2);
        EXPECT_TRUE(v1 <= v2);
        EXPECT_TRUE(v2 > v1);
        EXPECT_TRUE(v2 >= v1);
    }
    {
        Vec2 v1(1.f, 2.f);
        Vec2 v2(1.f, 2.f);

        EXPECT_FALSE(v1 != v2);
        EXPECT_TRUE(v1 == v2);
        EXPECT_FALSE(v1 < v2);
        EXPECT_TRUE(v1 <= v2);
        EXPECT_FALSE(v2 > v1);
        EXPECT_TRUE(v2 >= v1);
    }
}

TEST_F(VectorTests, Vec3_CheckConstructors) {
    {
        Vec3 v(1.f);

        EXPECT_FLOAT_EQ(v.x, 1.f);
        EXPECT_FLOAT_EQ(v.y, 1.f);
        EXPECT_FLOAT_EQ(v.z, 1.f);
    }

    {
        Vec3 v(1.f, 2.f, 3.f);

        EXPECT_FLOAT_EQ(v.x, 1.f);
        EXPECT_FLOAT_EQ(v.y, 2.f);
        EXPECT_FLOAT_EQ(v.z, 3.f);
    }

    {
        Vec3 v1(1.f, 2.f, 3.f);
        Vec3 v2(v1);

        EXPECT_FLOAT_EQ(v2.x, v1.x);
        EXPECT_FLOAT_EQ(v2.y, v1.y);
        EXPECT_FLOAT_EQ(v2.z, v1.z);
    }

    {
        Vec3 v1(1.f, 2.f, 3.f);
        Vec3 v2(0.f);
        v2 = v1;

        EXPECT_FLOAT_EQ(v2.x, v1.x);
        EXPECT_FLOAT_EQ(v2.y, v1.y);
        EXPECT_FLOAT_EQ(v2.z, v1.z);
    }

    {
        Vec2 v(1.f, 2.f);
        Vec3 v1(v, 3.f);

        EXPECT_FLOAT_EQ(v1.x, 1.f);
        EXPECT_FLOAT_EQ(v1.y, 2.f);
        EXPECT_FLOAT_EQ(v1.z, 3.f);
    }
}

TEST_F(VectorTests, Vec3_CheckOperators) {
    {
        Vec3 v(1.f, 2.f, 3.f);

        EXPECT_FLOAT_EQ(v[0], 1.f);
        EXPECT_FLOAT_EQ(v[1], 2.f);
        EXPECT_FLOAT_EQ(v[2], 3.f);
    }

    {
        Vec3 v(0.f);

        v[0] = 1.f;
        v[1] = 2.f;
        v[2] = 3.f;

        EXPECT_FLOAT_EQ(v[0], 1.f);
        EXPECT_FLOAT_EQ(v[1], 2.f);
        EXPECT_FLOAT_EQ(v[2], 3.f);
    }

    {
        Vec3 v1(1.f, 2.f, 3.f);
        Vec3 v2(4.f, 5.f, 6.f);

        Vec3 v3 = v1 + v2;

        EXPECT_FLOAT_EQ(v3.x, 5.f);
        EXPECT_FLOAT_EQ(v3.y, 7.f);
        EXPECT_FLOAT_EQ(v3.z, 9.f);
    }
    {
        Vec3 v1(1.f, 2.f, 3.f);
        Vec3 v2(4.f, 5.f, 6.f);

         v2 += v1;

        EXPECT_FLOAT_EQ(v2.x, 5.f);
        EXPECT_FLOAT_EQ(v2.y, 7.f);
        EXPECT_FLOAT_EQ(v2.z, 9.f);
    }

    {
        Vec3 v1(1.f, 2.f, 3.f);
        Vec3 v2(4.f, 5.f, 6.f);

        Vec3 v3 = v2 - v1;

        EXPECT_FLOAT_EQ(v3.x, 3.f);
        EXPECT_FLOAT_EQ(v3.y, 3.f);
        EXPECT_FLOAT_EQ(v3.z, 3.f);
    }
    {
        Vec3 v1(1.f, 2.f, 3.f);
        Vec3 v2(4.f, 5.f, 6.f);

        v2 -= v1;

        EXPECT_FLOAT_EQ(v2.x, 3.f);
        EXPECT_FLOAT_EQ(v2.y, 3.f);
        EXPECT_FLOAT_EQ(v2.z, 3.f);
    }
    {
        Vec3 v1(1.f, 2.f, 3.f);
        Vec3 v2 = -v1;

        EXPECT_FLOAT_EQ(v2.x, -v1.x);
        EXPECT_FLOAT_EQ(v2.y, -v1.y);
        EXPECT_FLOAT_EQ(v2.z, -v1.z);
    }

    {
        Vec3 v1(1.f, 2.f, 3.f);
        Vec3 v2;

        v2 = v1 * 0.5f;

        EXPECT_FLOAT_EQ(v2.x, 0.5f);
        EXPECT_FLOAT_EQ(v2.y, 1.f);
        EXPECT_FLOAT_EQ(v2.z, 1.5f);
    }
    {
        Vec3 v1(1.f, 2.f, 3.f);
        Vec3 v2;

        v2 = 0.5f * v1;

        EXPECT_FLOAT_EQ(v2.x, 0.5f);
        EXPECT_FLOAT_EQ(v2.y, 1.f);
        EXPECT_FLOAT_EQ(v2.z, 1.5f);
    }

    {
        Vec3 v1(1.f, 2.f, 3.f);
        Vec3 v2;

        v2 = v1 / 2.f;

        EXPECT_FLOAT_EQ(v2.x, 0.5f);
        EXPECT_FLOAT_EQ(v2.y, 1.f);
        EXPECT_FLOAT_EQ(v2.z, 1.5f);
    }

    {
        Vec3 v(1.f, 2.f, 3.f);

        v *= 0.5f;

        EXPECT_FLOAT_EQ(v.x, 0.5f);
        EXPECT_FLOAT_EQ(v.y, 1.f);
        EXPECT_FLOAT_EQ(v.z, 1.5f);
    }
    {
        Vec3 v(1.f, 2.f, 3.f);

        v /= 2.f;

        EXPECT_FLOAT_EQ(v.x, 0.5f);
        EXPECT_FLOAT_EQ(v.y, 1.f);
        EXPECT_FLOAT_EQ(v.z, 1.5f);
    }
}

TEST_F(VectorTests, Vec3_CheckMethods) {
    {
        Vec3 v(2.f);
        v.scale(Vec3(0.5, 0.25f, 0.1f));

        EXPECT_FLOAT_EQ(v.x, 1.f);
        EXPECT_FLOAT_EQ(v.y, 0.5f);
        EXPECT_FLOAT_EQ(v.z, 0.2f);
    }
    {
        Vec3 v(2.f);
        v.scale(0.5, 0.25f, 0.1f);

        EXPECT_FLOAT_EQ(v.x, 1.f);
        EXPECT_FLOAT_EQ(v.y, 0.5f);
        EXPECT_FLOAT_EQ(v.z, 0.2f);
    }

    {
        Vec3 v1(2.f);
        Vec3 v2 = v1.getScaled(Vec3(0.5, 0.25f, 0.1f));

        EXPECT_FLOAT_EQ(v2.x, 1.f);
        EXPECT_FLOAT_EQ(v2.y, 0.5f);
        EXPECT_FLOAT_EQ(v2.z, 0.2f);
    }
    {
        Vec3 v1(2.f);
        Vec3 v2 = v1.getScaled(0.5, 0.25f, 0.1f);

        EXPECT_FLOAT_EQ(v2.x, 1.f);
        EXPECT_FLOAT_EQ(v2.y, 0.5f);
        EXPECT_FLOAT_EQ(v2.z, 0.2f);
    }

    {
        Vec3 v(3.f, 4.f, 0.f);

        EXPECT_FLOAT_EQ(v.getLenghtSq(), 25.f);
        EXPECT_FLOAT_EQ(v.getLenght(), 5.f);
    }

    {
        Vec3 v1(2.f, 2.f, 2.f);

        Vec3 v2 = v1.getNormilized();

        EXPECT_FLOAT_EQ(v2.x, 0.57735026f);
        EXPECT_FLOAT_EQ(v2.y, 0.57735026f);
        EXPECT_FLOAT_EQ(v2.z, 0.57735026f);
    }
    {
        Vec3 v(2.f, 2.f, 2.f);
        v.normalize();

        EXPECT_FLOAT_EQ(v.x, 0.57735026f);
        EXPECT_FLOAT_EQ(v.y, 0.57735026f);
        EXPECT_FLOAT_EQ(v.z, 0.57735026f);
    }
}

TEST_F(VectorTests, Vec3_CheckCompare) {
    {
        Vec3 v1(0.f, 1.f, 2.f);
        Vec3 v2(3.f, 4.f, 5.f);

        EXPECT_TRUE(v1 != v2);
        EXPECT_FALSE(v1 == v2);
        EXPECT_TRUE(v1 < v2);
        EXPECT_TRUE(v1 <= v2);
        EXPECT_TRUE(v2 > v1);
        EXPECT_TRUE(v2 >= v1);
    }
    {
        Vec3 v1(1.f, 2.f, 3.f);
        Vec3 v2(1.f, 2.f, 3.f);

        EXPECT_FALSE(v1 != v2);
        EXPECT_TRUE(v1 == v2);
        EXPECT_FALSE(v1 < v2);
        EXPECT_TRUE(v1 <= v2);
        EXPECT_FALSE(v2 > v1);
        EXPECT_TRUE(v2 >= v1);
    }
}

TEST_F(VectorTests, Vec3_CheckDotProduct) {
    Vec3 v1(1.f, 2.f, 3.f);
    Vec3 v2(5.f, 5.f, 6.f);

    float val = v1.dot(v2);

    EXPECT_FLOAT_EQ(val, 33.f);
}

TEST_F(VectorTests, Vec3_CheckCrossProduct) {
    Vec3 v1(1.f, 0.f, 0.f);
    Vec3 v2(0.f, 1.f, 0.f);
    Vec3 v3 = v1.cross(v2);

    EXPECT_FLOAT_EQ(v3.x, 0.f);
    EXPECT_FLOAT_EQ(v3.y, 0.f);
    EXPECT_FLOAT_EQ(v3.z, 1.f);
}

TEST_F(VectorTests, Vec4_CheckConstructors) {
    {
        Vec4 v(1.f);

        EXPECT_FLOAT_EQ(v.x, 1.f);
        EXPECT_FLOAT_EQ(v.y, 1.f);
        EXPECT_FLOAT_EQ(v.z, 1.f);
        EXPECT_FLOAT_EQ(v.w, 1.f);
    }

    {
        Vec4 v(1.f, 2.f, 3.f, 4.f);

        EXPECT_FLOAT_EQ(v.x, 1.f);
        EXPECT_FLOAT_EQ(v.y, 2.f);
        EXPECT_FLOAT_EQ(v.z, 3.f);
        EXPECT_FLOAT_EQ(v.w, 4.f);
    }

    {
        Vec4 v1(1.f, 2.f, 3.f, 4.f);
        Vec4 v2(v1);

        EXPECT_FLOAT_EQ(v2.x, v1.x);
        EXPECT_FLOAT_EQ(v2.y, v1.y);
        EXPECT_FLOAT_EQ(v2.z, v1.z);
        EXPECT_FLOAT_EQ(v2.w, v1.w);
    }

    {
        Vec4 v1(1.f, 2.f, 3.f, 4.f);
        Vec4 v2(0.f);
        v2 = v1;

        EXPECT_FLOAT_EQ(v2.x, v1.x);
        EXPECT_FLOAT_EQ(v2.y, v1.y);
        EXPECT_FLOAT_EQ(v2.z, v1.z);
        EXPECT_FLOAT_EQ(v2.w, v1.w);
    }

    {
        Vec3 v(1.f, 2.f, 3.f);
        Vec4 v1(v, 4.f);

        EXPECT_FLOAT_EQ(v1.x, 1.f);
        EXPECT_FLOAT_EQ(v1.y, 2.f);
        EXPECT_FLOAT_EQ(v1.z, 3.f);
        EXPECT_FLOAT_EQ(v1.w, 4.f);
    }
}

TEST_F(VectorTests, Vec4_CheckOperators) {
    {
        Vec4 v(1.f, 2.f, 3.f, 4.f);

        EXPECT_FLOAT_EQ(v[0], 1.f);
        EXPECT_FLOAT_EQ(v[1], 2.f);
        EXPECT_FLOAT_EQ(v[2], 3.f);
        EXPECT_FLOAT_EQ(v[3], 4.f);
    }

    {
        Vec4 v(0.f);

        v[0] = 1.f;
        v[1] = 2.f;
        v[2] = 3.f;
        v[3] = 4.f;

        EXPECT_FLOAT_EQ(v[0], 1.f);
        EXPECT_FLOAT_EQ(v[1], 2.f);
        EXPECT_FLOAT_EQ(v[2], 3.f);
        EXPECT_FLOAT_EQ(v[3], 4.f);
    }

    {
        Vec4 v1(1.f, 2.f, 3.f, 4.f);
        Vec4 v2(5.f, 6.f, 7.f, 8.f);

        Vec4 v3 = v1 + v2;

        EXPECT_FLOAT_EQ(v3.x, 6.f);
        EXPECT_FLOAT_EQ(v3.y, 8.f);
        EXPECT_FLOAT_EQ(v3.z, 10.f);
        EXPECT_FLOAT_EQ(v3.w, 12.f);
    }
    {
        Vec4 v1(1.f, 2.f, 3.f, 4.f);
        Vec4 v2(5.f, 6.f, 7.f, 8.f);

         v2 += v1;

        EXPECT_FLOAT_EQ(v2.x, 6.f);
        EXPECT_FLOAT_EQ(v2.y, 8.f);
        EXPECT_FLOAT_EQ(v2.z, 10.f);
        EXPECT_FLOAT_EQ(v2.w, 12.f);
    }

    {
        Vec4 v1(1.f, 2.f, 3.f, 4.f);
        Vec4 v2(5.f, 6.f, 7.f, 8.f);

        Vec4 v3 = v2 - v1;

        EXPECT_FLOAT_EQ(v3.x, 4.f);
        EXPECT_FLOAT_EQ(v3.y, 4.f);
        EXPECT_FLOAT_EQ(v3.z, 4.f);
        EXPECT_FLOAT_EQ(v3.w, 4.f);
    }
    {
        Vec4 v1(1.f, 2.f, 3.f, 4.f);
        Vec4 v2(5.f, 6.f, 7.f, 8.f);

        v2 -= v1;

        EXPECT_FLOAT_EQ(v2.x, 4.f);
        EXPECT_FLOAT_EQ(v2.y, 4.f);
        EXPECT_FLOAT_EQ(v2.z, 4.f);
        EXPECT_FLOAT_EQ(v2.w, 4.f);
    }
    {
        Vec4 v1(1.f, 2.f, 3.f, 4.f);
        Vec4 v2 = -v1;

        EXPECT_FLOAT_EQ(v2.x, -v1.x);
        EXPECT_FLOAT_EQ(v2.y, -v1.y);
        EXPECT_FLOAT_EQ(v2.z, -v1.z);
        EXPECT_FLOAT_EQ(v2.w, -v1.w);
    }

    {
        Vec4 v1(1.f, 2.f, 3.f, 4.f);
        Vec4 v2;

        v2 = v1 * 0.5f;

        EXPECT_FLOAT_EQ(v2.x, 0.5f);
        EXPECT_FLOAT_EQ(v2.y, 1.f);
        EXPECT_FLOAT_EQ(v2.z, 1.5f);
        EXPECT_FLOAT_EQ(v2.w, 2.f);
    }
    {
        Vec4 v1(1.f, 2.f, 3.f, 4.f);
        Vec4 v2;

        v2 = 0.5f * v1;

        EXPECT_FLOAT_EQ(v2.x, 0.5f);
        EXPECT_FLOAT_EQ(v2.y, 1.f);
        EXPECT_FLOAT_EQ(v2.z, 1.5f);
        EXPECT_FLOAT_EQ(v2.w, 2.f);
    }

    {
        Vec4 v1(1.f, 2.f, 3.f, 4.f);
        Vec4 v2;

        v2 = v1 / 2.f;

        EXPECT_FLOAT_EQ(v2.x, 0.5f);
        EXPECT_FLOAT_EQ(v2.y, 1.f);
        EXPECT_FLOAT_EQ(v2.z, 1.5f);
        EXPECT_FLOAT_EQ(v2.w, 2.f);
    }

    {
        Vec4 v(1.f, 2.f, 3.f, 4.f);

        v *= 0.5f;

        EXPECT_FLOAT_EQ(v.x, 0.5f);
        EXPECT_FLOAT_EQ(v.y, 1.f);
        EXPECT_FLOAT_EQ(v.z, 1.5f);
        EXPECT_FLOAT_EQ(v.w, 2.f);
    }
    {
        Vec4 v(1.f, 2.f, 3.f, 4.f);

        v /= 2.f;

        EXPECT_FLOAT_EQ(v.x, 0.5f);
        EXPECT_FLOAT_EQ(v.y, 1.f);
        EXPECT_FLOAT_EQ(v.z, 1.5f);
        EXPECT_FLOAT_EQ(v.w, 2.f);
    }
}

TEST_F(VectorTests, Vec4_CheckMethods) {
    {
        Vec4 v(2.f);
        v.scale(Vec4(0.5, 0.25f, 0.1f, 0.f));

        EXPECT_FLOAT_EQ(v.x, 1.f);
        EXPECT_FLOAT_EQ(v.y, 0.5f);
        EXPECT_FLOAT_EQ(v.z, 0.2f);
        EXPECT_FLOAT_EQ(v.w, 0.f);
    }
    {
        Vec4 v(2.f);
        v.scale(0.5, 0.25f, 0.1f, 0.f);

        EXPECT_FLOAT_EQ(v.x, 1.f);
        EXPECT_FLOAT_EQ(v.y, 0.5f);
        EXPECT_FLOAT_EQ(v.z, 0.2f);
        EXPECT_FLOAT_EQ(v.w, 0.f);
    }

    {
        Vec4 v1(2.f);
        Vec4 v2 = v1.getScaled(Vec4(0.5, 0.25f, 0.1f, 0.f));

        EXPECT_FLOAT_EQ(v2.x, 1.f);
        EXPECT_FLOAT_EQ(v2.y, 0.5f);
        EXPECT_FLOAT_EQ(v2.z, 0.2f);
        EXPECT_FLOAT_EQ(v2.w, 0.0f);
    }
    {
        Vec4 v1(2.f);
        Vec4 v2 = v1.getScaled(0.5, 0.25f, 0.1f, 0.f);

        EXPECT_FLOAT_EQ(v2.x, 1.f);
        EXPECT_FLOAT_EQ(v2.y, 0.5f);
        EXPECT_FLOAT_EQ(v2.z, 0.2f);
        EXPECT_FLOAT_EQ(v2.w, 0.0f);
    }

    {
        Vec4 v(3.f, 4.f, 0.f, 0.f);

        EXPECT_FLOAT_EQ(v.getLenghtSq(), 25.f);
        EXPECT_FLOAT_EQ(v.getLenght(), 5.f);
    }

    {
        Vec4 v1(2.f);

        Vec4 v2 = v1.getNormilized();

        EXPECT_FLOAT_EQ(v2.x, 0.5f);
        EXPECT_FLOAT_EQ(v2.y, 0.5f);
        EXPECT_FLOAT_EQ(v2.z, 0.5f);
        EXPECT_FLOAT_EQ(v2.w, 0.5f);
    }
    {
        Vec4 v(2.f);
        v.normalize();

        EXPECT_FLOAT_EQ(v.x, 0.5f);
        EXPECT_FLOAT_EQ(v.y, 0.5f);
        EXPECT_FLOAT_EQ(v.z, 0.5f);
        EXPECT_FLOAT_EQ(v.w, 0.5f);
    }
}

TEST_F(VectorTests, Vec4_CheckCompare) {
    {
        Vec4 v1(0.f, 1.f, 2.f, 3.f);
        Vec4 v2(4.f, 5.f, 6.f, 7.f);

        EXPECT_TRUE(v1 != v2);
        EXPECT_FALSE(v1 == v2);
        EXPECT_TRUE(v1 < v2);
        EXPECT_TRUE(v1 <= v2);
        EXPECT_TRUE(v2 > v1);
        EXPECT_TRUE(v2 >= v1);
    }
    {
        Vec4 v1(0.f, 1.f, 2.f, 3.f);
        Vec4 v2(0.f, 1.f, 2.f, 3.f);

        EXPECT_FALSE(v1 != v2);
        EXPECT_TRUE(v1 == v2);
        EXPECT_FALSE(v1 < v2);
        EXPECT_TRUE(v1 <= v2);
        EXPECT_FALSE(v2 > v1);
        EXPECT_TRUE(v2 >= v1);
    }
}