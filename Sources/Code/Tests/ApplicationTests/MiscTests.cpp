#include "MiscTests.hpp"
#include "Core/Utils.hpp"

TEST_F(MiscTests, CheckSameTypeID) {
    auto intTypeId1 = GetTypeId<int>();
    auto intTypeId2 = GetTypeId<int>();
    ASSERT_EQ(intTypeId1, intTypeId2);
}

TEST_F(MiscTests, CheckSameTypeIDViaTypeDef) {
    typedef float TypedefFloat;
    auto floatTypeId1 = GetTypeId<float>();
    auto floatTypeId2 = GetTypeId<TypedefFloat>();
    ASSERT_EQ(floatTypeId1, floatTypeId2);
}

TEST_F(MiscTests, CheckDifferentTypes) {
    auto dobuleTypeId = GetTypeId<double>();
    auto charTypeId = GetTypeId<char>();
    ASSERT_NE(dobuleTypeId, charTypeId);
}

TEST_F(MiscTests, CheckNormalFormats) {
    const std::string s = "1";
    std::string str = StringFormat("%i %.1f %s %s", 1, 1.0f, "1", s);
    ASSERT_STREQ(str.c_str(), "1 1.0 1 1");
}

TEST_F(MiscTests, CheckVoidFormats) {
    const std::string testStr = "test_str";
    std::string resStr = StringFormat(testStr);
    ASSERT_EQ(resStr, testStr);
}

TEST_F(MiscTests, CheckApplyTuple) {
    std::tuple<int, int, int> t = {1, 2, 3};

    std::vector<int> order;

    ApplyTuple(t, [&order](int num){
        order.push_back(num);
    });

    ASSERT_EQ(order.size(), 3u);
    ASSERT_EQ(order[0], 1);
    ASSERT_EQ(order[1], 2);
    ASSERT_EQ(order[2], 3);

    order.clear();

    ApplyTupleReverse(t, [&order](int num){
        order.push_back(num);
    });

    ASSERT_EQ(order.size(), 3u);
    ASSERT_EQ(order[0], 3);
    ASSERT_EQ(order[1], 2);
    ASSERT_EQ(order[2], 1);
}