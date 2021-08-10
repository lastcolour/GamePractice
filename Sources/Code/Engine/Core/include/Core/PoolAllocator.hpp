#ifndef __POOL_ALLOCATOR_HPP__
#define __POOL_ALLOCATOR_HPP__

class PoolAllocator {
public:

    PoolAllocator(size_t objectSize, size_t objectsPerBlock);
    ~PoolAllocator();

    void* allocate();
    void deallocate(void* ptr);

private:

    struct ObjectPtr {
        void* next{nullptr};
    };

    struct BlockHeader {
        void* free;
    };

private:

    BlockHeader* allocateBlock();

private:

    PoolAllocator(const PoolAllocator&) = delete;
    PoolAllocator& operator=(const PoolAllocator&) = delete;

private:

    std::vector<std::unique_ptr<unsigned char[]>> blocks;
    size_t objSize;
    size_t objPerBlock;
};

#endif /* __POOL_ALLOCATOR_HPP__ */