#include <cassert>

namespace Memory {

namespace Impl {

size_t getBlockSize(size_t objSize, size_t objAlign, size_t objCount) {
    auto blockHdrSize = sizeof(BaseMemoryPoolAllocator::BlockHeader);
    auto objHdrSize = sizeof(BaseMemoryPoolAllocator::ObjectHeader);

    return (objSize + objHdrSize) * objCount + blockHdrSize;
}

BaseMemoryPoolAllocator::ObjectHeader* getObjectHdr(BaseMemoryPoolAllocator::BlockHeader* blockHrd, BaseUintT objectId, size_t objSize, size_t objAlign) {
    auto blockHdrSize = sizeof(BaseMemoryPoolAllocator::BlockHeader);
    auto objHdrSize = sizeof(BaseMemoryPoolAllocator::ObjectHeader);

    auto ptr = reinterpret_cast<uint8_t*>(blockHrd);
    ptr = (ptr + blockHdrSize) + objectId * (objSize + objHdrSize);

    return reinterpret_cast<BaseMemoryPoolAllocator::ObjectHeader*>(ptr);
}

void* getObjectFromObjectHrd(BaseMemoryPoolAllocator::ObjectHeader* objHdr, size_t objSize, size_t objAlign) {
    auto objHdrSize = sizeof(BaseMemoryPoolAllocator::ObjectHeader);

    auto ptr = reinterpret_cast<uint8_t*>(objHdr) + objHdrSize;

    if(reinterpret_cast<uintptr_t>(ptr) % objAlign) {
        assert(false && "Invalid object alignment");
    }

    return ptr;
}

bool tryGetObjectHdrInBlock(BaseMemoryPoolAllocator::BlockHeader* blockHdr,
    void* ptr, size_t objSize, size_t objAlign, size_t objInBlock,
    BaseMemoryPoolAllocator::ObjectHeader*& objHdr, BaseUintT& objIdx) {

    auto blockHdrSize = sizeof(BaseMemoryPoolAllocator::BlockHeader);
    auto objHdrSize = sizeof(BaseMemoryPoolAllocator::ObjectHeader);

    auto blockSz = getBlockSize(objSize, objAlign, objInBlock);
    if(reinterpret_cast<uintptr_t>(ptr) < reinterpret_cast<uintptr_t>(blockHdr)
        || reinterpret_cast<uintptr_t>(ptr) > (reinterpret_cast<uintptr_t>(blockHdr) + blockSz)) {
        return false;
    }

    objHdr = reinterpret_cast<BaseMemoryPoolAllocator::ObjectHeader*>(
        reinterpret_cast<uint8_t*>(ptr) - objHdrSize);
    objIdx = static_cast<BaseUintT>((
        reinterpret_cast<uintptr_t>(objHdr) - reinterpret_cast<uintptr_t>(blockHdr)) / (objHdrSize + objSize));

    assert(((reinterpret_cast<uintptr_t>(objHdr) - reinterpret_cast<uintptr_t>(blockHdr) - blockHdrSize) % (objHdrSize + objSize)) == 0
        && "Invalid aligment");

    return true;
}

const BaseUintT OBJECT_OCCUPIED_ID = std::numeric_limits<BaseUintT>::max() - 1;
const BaseUintT OBJECT_END_ID = std::numeric_limits<BaseUintT>::max() - 2;

BaseUintT BaseMemoryPoolAllocator::GetPoolId(void* ptr) {
    auto objHdr = reinterpret_cast<ObjectHeader*>(
        static_cast<uint8_t*>(ptr) - sizeof(ObjectHeader));
    return objHdr->poolId;
}

BaseMemoryPoolAllocator::BaseMemoryPoolAllocator() :
    objSize(0),
    objAlign(1),
    id(0),
    useCentralAllocator(true) {
}

BaseMemoryPoolAllocator::BaseMemoryPoolAllocator(size_t objectSize, BaseUintT objectAlign) :
    objSize(objectSize),
    objAlign(objectAlign),
    id(0),
    useCentralAllocator(true) {
}

BaseMemoryPoolAllocator::~BaseMemoryPoolAllocator() {
    for(auto& b : blocks) {
        if(useCentralAllocator) {
            auto blockHdrd = reinterpret_cast<BlockHeader*>(b);
            GetEnv()->GetMemoryAllocator()->deallocate(blockHdrd);
        } else {
            delete[] b;
        }
    }
    blocks.clear();
}

void* BaseMemoryPoolAllocator::allocate() {
    MemLocker memLocker;
    return allocateUnsafe();
}

void BaseMemoryPoolAllocator::deallocate(void* ptr) {
    MemLocker memLocker;
    deallocateUnsafe(ptr);
}

void* BaseMemoryPoolAllocator::allocateUnsafe() {
    BlockHeader* blockHrd = nullptr;
    for(auto& b : blocks) {
        auto currBlockHdr = reinterpret_cast<BlockHeader*>(b);
        if(currBlockHdr->freeIdx != OBJECT_END_ID) {
            blockHrd = currBlockHdr;
            break;
        }
    }
    if(!blockHrd) {
        blockHrd = allocateBlock(blocks.size());
        blocks.emplace_back(reinterpret_cast<uint8_t*>(blockHrd));
    }

    auto freeObjHdr = getObjectHdr(blockHrd, blockHrd->freeIdx, objSize, objAlign);
    blockHrd->freeIdx = freeObjHdr->nextIdx;
    blockHrd->size += 1;
    freeObjHdr->nextIdx = OBJECT_OCCUPIED_ID;

    return getObjectFromObjectHrd(freeObjHdr, objSize, objAlign);
}

void BaseMemoryPoolAllocator::deallocateUnsafe(void* ptr) {
    if(!ptr) {
        return;
    }
    for(size_t i = 0, sz = blocks.size(); i < sz; ++i) {
        auto blockHdr = reinterpret_cast<BlockHeader*>(blocks[i]);
        ObjectHeader* objHdr = nullptr;
        BaseUintT objIdx = 0;
        if(!tryGetObjectHdrInBlock(blockHdr, ptr, objSize, objAlign, getObjectsInBlock(i),
            objHdr, objIdx)) {
            continue;
        }

        assert(objHdr->nextIdx == OBJECT_OCCUPIED_ID && "Object isn't occupied");

        objHdr->nextIdx = blockHdr->freeIdx;
        blockHdr->freeIdx = objIdx;
        blockHdr->size -= 1;

        if(blockHdr->size == 0 && canRemoveBlocks()) {
            std::swap(blocks[i], blocks.back());
            if(useCentralAllocator) {
                GetEnv()->GetMemoryAllocator()->deallocateUnsafe(blockHdr);
            } else {
                delete[] blocks[i];
            }
            blocks.pop_back();
        }
    }
}

BaseMemoryPoolAllocator::BlockHeader* BaseMemoryPoolAllocator::allocateBlock(size_t blockId) {
    auto objPerBlock = getObjectsInBlock(blockId);
    size_t memSize = getBlockSize(objSize, objAlign, objPerBlock);

    void* ptr = nullptr;
    if(useCentralAllocator) {
        ptr = static_cast<uint8_t*>(GetEnv()->GetMemoryAllocator()->allocateUnsafe(memSize));
    } else {
        ptr = new uint8_t [memSize];
    }

    if(!ptr) {
        assert(false && "Can't allocate memory block");
        return nullptr;
    }

    auto blockHdr = reinterpret_cast<BlockHeader*>(ptr);
    blockHdr->freeIdx = 0;
    blockHdr->size = 0;

    for(BaseUintT i = 0; i < (objPerBlock - 1); ++i) {
        auto currObj = getObjectHdr(blockHdr, i, objSize, objAlign);
        currObj->nextIdx = i + 1;
        currObj->poolId = id;
    }
    auto lastObjHdr = getObjectHdr(blockHdr, objPerBlock - 1, objSize, objAlign);
    lastObjHdr->nextIdx = OBJECT_END_ID;
    lastObjHdr->poolId = id;

    return blockHdr;
}

void BaseMemoryPoolAllocator::setObjectSize(size_t objectSize) {
    assert(blocks.empty() && "Allocator already use another value");
    assert(!(objSize % sizeof(ObjectHeader)) && "Invalid aligment");
    objSize = objectSize;
}

void BaseMemoryPoolAllocator::setPoolId(BaseUintT poolId) {
    assert(blocks.empty() && "Allocator already use another value");
    id = poolId;
}

void BaseMemoryPoolAllocator::setUseCentralAllocator(bool flag) {
    assert(blocks.empty() && "Allocator already use another value");
    useCentralAllocator = flag;
}

float BaseMemoryPoolAllocator::getFillRatio() const {
    size_t objCount = 0;
    size_t totalObjCount = 0;
    {
        MemLocker memLocker;
        for(size_t i = 0, sz = blocks.size(); i < sz; ++i) {
            auto blockHrd = reinterpret_cast<BlockHeader*>(blocks[i]);
            totalObjCount += getObjectsInBlock(i);
            objCount += blockHrd->size;
        }
    }
    if(totalObjCount == 0) {
        return 1.f;
    }
    return objCount / static_cast<float>(totalObjCount);
}

size_t BaseMemoryPoolAllocator::getNumBlocks() const {
    MemLocker memLocker;
    return blocks.size();
}

size_t BaseMemoryPoolAllocator::getNumObjects() const {
    size_t res = 0;
    {
        MemLocker memLocker;
        for(auto& b : blocks) {
            auto blockHrd = reinterpret_cast<BlockHeader*>(b);
            res += blockHrd->size;
        }
    }
    return res;
}

size_t BaseMemoryPoolAllocator::getAllocatedMemorySize() const {
    size_t res = 0;
    {
        MemLocker memLocker;
        for(size_t i = 0, sz = blocks.size(); i < sz; ++i) {
            res += getBlockSize(objSize, objAlign, getObjectsInBlock(i));
        }
    }
    return res;
}

size_t BaseMemoryPoolAllocator::getObjectSize() const {
    return objSize;
}

void BaseMemoryPoolAllocator::visitEachObject(void(*func)(void*)) {
    MemLocker memLocker;

    for(size_t i = 0, sz = blocks.size(); i < sz; ++i) {
        auto& b = blocks[i];
        auto objCount = getObjectsInBlock(i);
        for(BaseUintT j = 0; j < objCount; ++j) {
            auto objHdr = getObjectHdr(reinterpret_cast<BlockHeader*>(b), j, objSize, objAlign);
            if(objHdr->nextIdx == OBJECT_OCCUPIED_ID) {
                func(getObjectFromObjectHrd(objHdr, objSize, objAlign));
            }
        }
    }
}

} // namespace Impl

} // namespace Memory