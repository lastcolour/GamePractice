#include "CycleArrayTests.hpp"
#include "Core/CycleArray.hpp"

TEST_F(CycleArrayTests, CheckCycleArray) {
    Core::CycleArray<int> cycleArray(5);

    for(int i = 0; i < 5; ++i) {
        cycleArray.insert(i + 1);
        EXPECT_EQ(cycleArray[i], i + 1);
        EXPECT_EQ(cycleArray.size(), i + 1);
    }

    for(int i = 0; i < 5; ++i) {
        cycleArray.insert(i);
        EXPECT_EQ(cycleArray[4], i);
        EXPECT_EQ(cycleArray.size(), 5);
    }
}