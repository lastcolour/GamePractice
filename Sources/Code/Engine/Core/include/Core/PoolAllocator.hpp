#ifndef __POOL_ALLOCATOR_HPP__
#define __POOL_ALLOCATOR_HPP__

namespace Memory {

using BaseUintT = uint32_t;

template<BaseUintT GrowSize>
class FixedGrowPolicy {
public:

    static BaseUintT getObjectsInBlock(size_t blockId) {
        return GrowSize;
    }

    static bool canRemoveBlocks() {
        return true;
    }

};

class AdaptiveGrowPolicy {
public:

    static BaseUintT getObjectsInBlock(size_t blockId) {
        BaseUintT sizes[] = {1, 16, 16, 32, 32, 64, 64, 256};
        auto sizeCount = sizeof(sizes) / sizeof(BaseUintT);
        return sizes[std::min(sizeCount - 1, blockId)];
    }

    static bool canRemoveBlocks() {
        return false;
    }

};

namespace Impl {

class BaseMemoryPoolAllocator {
public:

    static BaseUintT GetPoolId(void* ptr);

public:

    struct ObjectHeader {
        BaseUintT nextIdx;
        BaseUintT poolId;
    };

    struct BlockHeader {
        BaseUintT freeIdx;
        BaseUintT size;
    };

public:

    BaseMemoryPoolAllocator();
    BaseMemoryPoolAllocator(size_t objectSize, BaseUintT objectAlign);
    virtual ~BaseMemoryPoolAllocator();

    void setObjectSize(size_t objectSize);
    void setPoolId(BaseUintT poolId);
    void setUseCentralAllocator(bool flag);

    void* allocate();
    void deallocate(void* ptr);

    float getFillRatio() const;
    size_t getNumBlocks() const;
    size_t getNumObjects() const;
    size_t getAllocatedMemorySize() const;
    size_t getObjectSize() const;

    void visitEachObject(void(*func)(void*));

protected:

    virtual BaseUintT getObjectsInBlock(size_t blockId) const = 0;
    virtual bool canRemoveBlocks() const = 0;

private:

    BlockHeader* allocateBlock(size_t blockId);

private:

    BaseMemoryPoolAllocator(const BaseMemoryPoolAllocator&) = delete;
    BaseMemoryPoolAllocator& operator=(const BaseMemoryPoolAllocator&) = delete;

private:

    std::vector<uint8_t*> blocks;
    size_t objSize;
    BaseUintT objAlign;
    BaseUintT id;
    bool useCentralAllocator;
};

} // namesapce Impl

template<typename PolicyT>
class PoolAllocator : public Impl::BaseMemoryPoolAllocator {
public:

    PoolAllocator(size_t objectSize, BaseUintT objectAlign) :
        Impl::BaseMemoryPoolAllocator(objectSize, objectAlign) {}
    PoolAllocator() = default;
    virtual ~PoolAllocator() = default;

protected:

    BaseUintT getObjectsInBlock(size_t blockId) const override {
        return PolicyT::getObjectsInBlock(blockId);
    }

    bool canRemoveBlocks() const override {
        return PolicyT::canRemoveBlocks();
    }
};

} // namespace Memory

#endif /* __POOL_ALLOCATOR_HPP__ */