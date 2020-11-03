#include "QuaternionTests.hpp"
#include "Math/Quaternion.hpp"

TEST_F(QuaternionTests, CheckIndenity) {
    Quat q = Quat::Indentity();

    {
        EXPECT_FLOAT_EQ(q.getLenght(), 1.f);
        EXPECT_FLOAT_EQ(q.getLenghtSq(), 1.f);
    }

    {
        Vec3 pt(1.f, 2.f, 3.f);
        Vec3 resPt = q * pt;
        EXPECT_FLOAT_EQ(resPt.x, 1.f);
        EXPECT_FLOAT_EQ(resPt.y, 2.f);
        EXPECT_FLOAT_EQ(resPt.z, 3.f);
    }

    {
        Quat otherQ = Quat::Indentity();
        Quat resQ = q * otherQ;

        EXPECT_FLOAT_EQ(resQ.x, 0.f);
        EXPECT_FLOAT_EQ(resQ.y, 0.f);
        EXPECT_FLOAT_EQ(resQ.z, 0.f);
        EXPECT_FLOAT_EQ(resQ.w, 1.f);
    }

    {
        auto rotMat = q.toMat3();

        EXPECT_EQ(rotMat[0], Vec3(1.f, 0.f, 0.f));
        EXPECT_EQ(rotMat[1], Vec3(0.f, 1.f, 0.f));
        EXPECT_EQ(rotMat[2], Vec3(0.f, 0.f, 1.f));
    }

    {
        auto rotMat = q.toMat4();

        EXPECT_EQ(rotMat[0], Vec4(1.f, 0.f, 0.f, 0.f));
        EXPECT_EQ(rotMat[1], Vec4(0.f, 1.f, 0.f, 0.f));
        EXPECT_EQ(rotMat[2], Vec4(0.f, 0.f, 1.f, 0.f));
        EXPECT_EQ(rotMat[3], Vec4(0.f, 0.f, 0.f, 1.f));
    }

    {
        Vec3 axis(0.f);
        float angle = -1.f;

        q.getAxisAngle(axis, angle);

        EXPECT_FLOAT_EQ(angle, 0.f);

        EXPECT_FLOAT_EQ(axis.getLenght(), 1.f);
    }

    {
        Vec3 angles = q.getEurelAngels();

        EXPECT_FLOAT_EQ(angles.x, 0.f);
        EXPECT_FLOAT_EQ(angles.y, 0.f);
        EXPECT_FLOAT_EQ(angles.z, 0.f);
    }
}