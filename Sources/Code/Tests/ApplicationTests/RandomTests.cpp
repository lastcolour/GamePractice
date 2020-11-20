#include "RandomTests.hpp"
#include "Math/Random.hpp"

TEST_F(RandomTests, CheckIntGenerator) {
    const int N = 1000;
    const int MinVal = 0;
    const int MaxVal = 10;

    auto intGenerator = Math::RandomIntGenerator(MinVal, MaxVal);

    std::vector<int> results(MaxVal + 1, 0);

    for(int i = 0; i < N; ++i) {
        int val = intGenerator.generate();
        ASSERT_GE(val, MinVal);
        ASSERT_LE(val, MaxVal);
        ++results[val];
    }

    int count = N / (MaxVal + 1);
    int window = count / 5;

    for(int i = 0; i < MaxVal + 1; ++i) {
        int resCount = results[i];
        EXPECT_GE(resCount, count - window);
        EXPECT_LE(resCount, count + window);
    }
}

TEST_F(RandomTests, CheckFloatGenerator) {
    const int N = 1000;
    const float MinVal = -5.f;
    const float MaxVal = 5.f;

    auto floatGenerator = Math::RandomFloatGenerator(MinVal, MaxVal);

    for(int i = 0; i < N; ++i) {
        float val = floatGenerator.generate();
        ASSERT_GE(val, MinVal);
        ASSERT_LT(val, MaxVal);
    }
}