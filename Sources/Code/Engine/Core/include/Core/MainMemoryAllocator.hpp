#ifndef __MAIN_MEMORY_ALLOCATOR_HPP__
#define __MAIN_MEMORY_ALLOCATOR_HPP__

#include "Core/PoolAllocator.hpp"

namespace Memory {

class MainMemoryAllocator {
public:

    static const BaseUintT MAX_FIXED_BLOCK_SIZE = 11;

public:

    MainMemoryAllocator();
    ~MainMemoryAllocator();

    void* allocate(size_t size);
    void deallocate(void* ptr);
    void update(float dt);

private:

    MainMemoryAllocator(const MainMemoryAllocator&) = delete;
    MainMemoryAllocator& operator=(const MainMemoryAllocator&) = delete;

private:

    struct BigChunk {
        uint8_t* ptr;
        size_t size;
        float removeDelay;
    };

private:

    PoolAllocator<AdaptiveGrowPolicy> fixedSizePools[MAX_FIXED_BLOCK_SIZE];
    std::vector<BigChunk> bigChunks;
    std::mutex mutex;
};

} // namespace Memory

#endif /* __MAIN_MEMORY_ALLOCATOR_HPP__ */