#include "TransformTests.hpp"
#include "Math/Transform.hpp"

TEST_F(TransformTests, CheckGetAddDelta) {
    Transform tm1;
    tm1.pt = Vec3(0.f, 0.f, 0.f);
    tm1.scale = Vec3(1.f);
    tm1.quat.setAxisAngle(Vec3(0.f, 0.f, 1.f), Math::PI / 4.f);

    Transform tm2;
    tm2.pt = Vec3(1.f, 2.f, 3.f);
    tm2.scale = Vec3(3.f, 2.f, 1.f);
    tm1.quat.setAxisAngle(Vec3(0.f, 0.f, 1.f), 3.f * Math::PI / 4.f);

    Transform deltaTm = tm2.getDelta(tm1);
    tm1.addDelta(deltaTm);

    EXPECT_FLOAT_EQ(tm1.pt.x, tm2.pt.x);
    EXPECT_FLOAT_EQ(tm1.pt.y, tm2.pt.y);
    EXPECT_FLOAT_EQ(tm1.pt.z, tm2.pt.z);

    EXPECT_FLOAT_EQ(tm1.scale.x, tm2.scale.x);
    EXPECT_FLOAT_EQ(tm1.scale.y, tm2.scale.y);
    EXPECT_FLOAT_EQ(tm1.scale.z, tm2.scale.z);

    EXPECT_FLOAT_EQ(tm1.quat.x, tm2.quat.x);
    EXPECT_FLOAT_EQ(tm1.quat.y, tm2.quat.y);
    EXPECT_FLOAT_EQ(tm1.quat.z, tm2.quat.z);
    EXPECT_FLOAT_EQ(tm1.quat.w, tm2.quat.w);
}

TEST_F(TransformTests, CheckInverse) {
    Transform tm;
    tm.pt = Vec3(1.f, 2.f, 3.f);
    tm.scale = Vec3(3.f, 2.f, 1.f);
    tm.quat.setAxisAngle(Vec3(0.f, 0.f, 1.f), 3.f * Math::PI / 4.f);

    Transform tmInv = tm.getInversed();
    tmInv.addDelta(tm);

    EXPECT_FLOAT_EQ(tmInv.pt.x, 0.f);
    EXPECT_FLOAT_EQ(tmInv.pt.y, 0.f);
    EXPECT_FLOAT_EQ(tmInv.pt.z, 0.f);

    EXPECT_FLOAT_EQ(tmInv.scale.x, 1.f);
    EXPECT_FLOAT_EQ(tmInv.scale.y, 1.f);
    EXPECT_FLOAT_EQ(tmInv.scale.z, 1.f);

    EXPECT_FLOAT_EQ(tmInv.quat.x, 0.f);
    EXPECT_FLOAT_EQ(tmInv.quat.y, 0.f);
    EXPECT_FLOAT_EQ(tmInv.quat.z, 0.f);
    EXPECT_FLOAT_EQ(tmInv.quat.w, 1.f);
}