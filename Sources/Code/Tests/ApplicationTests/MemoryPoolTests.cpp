#include "MemoryPoolTests.hpp"
#include "Core/PoolAllocator.hpp"

#include <thread>

TEST_F(MemoryPoolTests, CheckAllocateReuse) {
    Memory::PoolAllocator<Memory::AdaptiveGrowPolicy> allocator(32, 1);

    {
        EXPECT_FLOAT_EQ(1.f, allocator.getFillRatio());
        EXPECT_EQ(0, allocator.getNumBlocks());
        EXPECT_EQ(0, allocator.getNumObjects());
        EXPECT_EQ(0, allocator.getAllocatedMemorySize());
    }

    auto ptr1 = allocator.allocate();
    EXPECT_TRUE(ptr1);

    {
        EXPECT_FLOAT_EQ(1.f, allocator.getFillRatio());
        EXPECT_EQ(1, allocator.getNumBlocks());
        EXPECT_EQ(1, allocator.getNumObjects());
        EXPECT_EQ(8 + 8 + 32, allocator.getAllocatedMemorySize());
    }

    allocator.deallocate(ptr1);

    {
        EXPECT_FLOAT_EQ(0.f, allocator.getFillRatio());
        EXPECT_EQ(1, allocator.getNumBlocks());
        EXPECT_EQ(0, allocator.getNumObjects());
    }

    auto ptr2 = allocator.allocate();
    EXPECT_TRUE(ptr2);

    EXPECT_EQ(ptr1, ptr2);
}

TEST_F(MemoryPoolTests, CheckBlockAllocation) {
    Memory::PoolAllocator<Memory::FixedGrowPolicy<16>> allocator(32, 1);

    for(int i = 0; i < 8; ++i) {
        auto ptr1 = allocator.allocate();
        auto ptr2 = allocator.allocate();

        EXPECT_EQ(ptr2, static_cast<unsigned char*>(ptr1) + 32 + 8);
    }
}

TEST_F(MemoryPoolTests, CheckRemoveBlocks) {
    Memory::PoolAllocator<Memory::FixedGrowPolicy<1>> allocator(32, 1);

    auto ptr = allocator.allocate();

    EXPECT_EQ(allocator.getNumBlocks(), 1);

    allocator.deallocate(ptr);

    EXPECT_EQ(allocator.getNumBlocks(), 0);
}

TEST_F(MemoryPoolTests, CheckAsyncPoolUse) {
    Memory::PoolAllocator<Memory::AdaptiveGrowPolicy> allocator(32, 8);

    std::atomic<bool> runFlag(true);
    std::atomic<int> activeThread(16);

    std::vector<std::thread> workers;
    for(int i = 0; i < 16; ++i) {
        std::thread t([&allocator, &runFlag, &activeThread](){
            while(runFlag.load()) {
                auto obj = allocator.allocate();
                ASSERT_TRUE(obj);
                allocator.deallocate(obj);
            }
            activeThread.fetch_sub(1);
        });
        t.detach();
        workers.push_back(std::move(t));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    runFlag.store(false);

    while(activeThread.load() != 0) {
        std::this_thread::yield();
    }

    EXPECT_EQ(0, allocator.getNumObjects());
}