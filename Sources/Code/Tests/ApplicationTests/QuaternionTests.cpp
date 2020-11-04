#include "QuaternionTests.hpp"
#include "Math/Quaternion.hpp"
#include "Math/MatrixTransform.hpp"

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
        Vec3 angles = q.getEulerAngels();

        EXPECT_FLOAT_EQ(angles.x, 0.f);
        EXPECT_FLOAT_EQ(angles.y, 0.f);
        EXPECT_FLOAT_EQ(angles.z, 0.f);
    }
}

TEST_F(QuaternionTests, CheckAxisAngle) {
    Quat q;
    q.setAxisAngle(Vec3(0.f, 0.f, 1.f), Math::PI / 2.f);

    {
        Vec3 axis(0.f);
        float angle = 0.f;
        q.getAxisAngle(axis, angle);

        EXPECT_FLOAT_EQ(axis.x, 0.f);
        EXPECT_FLOAT_EQ(axis.y, 0.f);
        EXPECT_FLOAT_EQ(axis.z, 1.f);

        EXPECT_FLOAT_EQ(angle, Math::PI / 2.f);
    }

    {
        EXPECT_FLOAT_EQ(q.getLenght(), 1.f);
    }

    {
        Vec3 angles =  q.getEulerAngels();

        EXPECT_FLOAT_EQ(angles.x, 0.f);
        EXPECT_FLOAT_EQ(angles.y, 0.f);
        EXPECT_FLOAT_EQ(angles.z, 0.f);
    }

    {
        Mat4 quatMat = q.toMat4();
        Quat qFromMat;
        qFromMat.setRotationMat(quatMat);

        EXPECT_FLOAT_EQ(qFromMat.x, q.x);
        EXPECT_FLOAT_EQ(qFromMat.y, q.y);
        EXPECT_FLOAT_EQ(qFromMat.z, q.z);
        EXPECT_FLOAT_EQ(qFromMat.w, q.w);
    }

    {
        Vec3 pt = Vec3(1.f, 1.f, 0.f);
        Vec3 rotPt = q * pt;

        EXPECT_FLOAT_EQ(rotPt.x, -1.f);
        EXPECT_FLOAT_EQ(rotPt.y, 1.f);
        EXPECT_FLOAT_EQ(rotPt.z, 0.f);
    }

    {
        Vec3 pt = Vec3(1.f, 1.f, 0.f);
        Mat3 rotMat = q.toMat3();
        Vec3 rotPt = rotMat * pt;

        EXPECT_FLOAT_EQ(rotPt.x, -1.f);
        EXPECT_FLOAT_EQ(rotPt.y, 1.f);
        EXPECT_FLOAT_EQ(rotPt.z, 0.f);
    }

    {
        Vec3 axis(0.f);
        float angle = 0.f;
        q.getAxisAngle(axis, angle);

        Mat4 rotMat(1.f);
        Math::Rotate(rotMat, axis, angle);

        Vec3 pt(1.f);

        Vec4 matRotPt =  rotMat * Vec4(pt, 1.f);
        Vec3 quatRotPt = q * pt;

        EXPECT_FLOAT_EQ(matRotPt.x, quatRotPt.x);
        EXPECT_FLOAT_EQ(matRotPt.y, quatRotPt.y);
        EXPECT_FLOAT_EQ(matRotPt.z, quatRotPt.z);
        EXPECT_FLOAT_EQ(matRotPt.w, 1.f);
    }
}

TEST_F(QuaternionTests, CheckRotationBetween) {
    Vec3 p1(0.f, 1.f, 0.f);
    Vec3 p2(1.f, 0.f, 0.f);

    Quat q;
    q.setRotationBetween(p1, p2);

    {
        Vec3 axis(0.f);
        float angle = 0.f;
        q.getAxisAngle(axis, angle);

        EXPECT_FLOAT_EQ(axis.x, 0.f);
        EXPECT_FLOAT_EQ(axis.y, 0.f);
        EXPECT_FLOAT_EQ(axis.z, -1.f);

        EXPECT_FLOAT_EQ(angle, Math::PI / 2.f);
    }

    {
        EXPECT_FLOAT_EQ(q.getLenght(), 1.f);
    }

    {
        Vec3 resPt = q * p1;

        EXPECT_FLOAT_EQ(resPt.x, p2.x);
        EXPECT_FLOAT_EQ(resPt.y, p2.y);
        EXPECT_FLOAT_EQ(resPt.z, p2.z);
    }
}

TEST_F(QuaternionTests, CheckCombine) {
    {
        Quat q1;
        q1.setAxisAngle(Vec3(0.f, 0.f, 1.f), -Math::PI / 2.f);

        Quat q2;
        q2.setAxisAngle(Vec3(0.f, 0.f, -1.f), -Math::PI / 2.f);

        Quat q3 = q1 * q2;

        EXPECT_FLOAT_EQ(q3.x, 0.f);
        EXPECT_FLOAT_EQ(q3.y, 0.f);
        EXPECT_FLOAT_EQ(q3.z, 0.f);
        EXPECT_FLOAT_EQ(q3.w, 1.f);
    }
}

TEST_F(QuaternionTests, CheckInverse) {
    Quat q;
    q.setAxisAngle(Vec3(0.f, 1.f, 0.f), Math::PI / 2.f);

    {
        Vec3 pt(1.f);

        Quat qInv = q.getInversed();

        Vec3 resPt1 = qInv * q * pt;
        Vec3 resPt2 = q * qInv * pt;

        EXPECT_FLOAT_EQ(resPt1.x, resPt2.x);
        EXPECT_FLOAT_EQ(resPt1.y, resPt2.y);
        EXPECT_FLOAT_EQ(resPt1.z, resPt2.z);
    }
}

TEST_F(QuaternionTests, CheckSLerp) {
    ASSERT_TRUE(false);
}

TEST_F(QuaternionTests, CheckNLerp) {
    ASSERT_TRUE(false);
}