#include "MemoryPoolTests.hpp"
#include "Core/PoolAllocator.hpp"

TEST_F(MemoryPoolTests, CheckAllocateReuse) {
    PoolAllocator allocator(32, 1);

    auto ptr1 = allocator.allocate();
    EXPECT_TRUE(ptr1);

    allocator.deallocate(ptr1);

    auto ptr2 = allocator.allocate();
    EXPECT_TRUE(ptr2);

    EXPECT_EQ(ptr1, ptr2);
}

TEST_F(MemoryPoolTests, CheckBlockAllocation) {
    PoolAllocator allocator(32, 2);

    for(int i = 0; i < 64; ++i) {
        auto ptr1 = allocator.allocate();
        auto ptr2 = allocator.allocate();

        EXPECT_EQ(ptr2, static_cast<unsigned char*>(ptr1) + 32);
    }
}