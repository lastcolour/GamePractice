#include <cassert>

namespace {

float BIG_CHUNK_REMOVE_DELAY = 8.f;

const Memory::BaseUintT CHUNKS_POOL_ID = 256;

const size_t FIXED_POOL_SIZES[] = {
    16,
    32,
    64,
    128,
    256,
    512,
    1024,
    2048,
    4096,
    8192,
    16384
};

struct ChunkHeader {
    Memory::BaseUintT i;
    Memory::BaseUintT poolId;
};

static_assert(sizeof(ChunkHeader) == sizeof(Memory::Impl::BaseMemoryPoolAllocator::BlockHeader));
static_assert(sizeof(ChunkHeader) == sizeof(Memory::Impl::BaseMemoryPoolAllocator::ObjectHeader));

} // namespace

namespace Memory {

MainMemoryAllocator::MainMemoryAllocator() {
    assert(sizeof(FIXED_POOL_SIZES) / sizeof(size_t) == MAX_FIXED_BLOCK_SIZE && "Invalid amount of pool sizes");
    for(Memory::BaseUintT i = 0; i < MAX_FIXED_BLOCK_SIZE; ++i) {
        fixedSizePools[i].setPoolId(i);
        fixedSizePools[i].setObjectSize(FIXED_POOL_SIZES[i]);
        fixedSizePools[i].setUseCentralAllocator(false);
    }
}

MainMemoryAllocator::~MainMemoryAllocator() {
    for(auto& chunk : bigChunks) {
        assert(chunk.removeDelay > 0.f && "Not freed memory detected");
        delete[] chunk.ptr;
    }
}

void* MainMemoryAllocator::allocate(size_t size) {
    void* ptr = nullptr;
    {
        memLock();
        ptr = allocateUnsafe(size);
        memUnlock();
    }
    return ptr;
}

void MainMemoryAllocator::deallocate(void* ptr) {
    if(!ptr) {
        return;
    }
    memLock();
    deallocateUnsafe(ptr);
    memUnlock();
}

void* MainMemoryAllocator::allocateUnsafe(size_t size) {
    for(uint32_t i = 0; i < MAX_FIXED_BLOCK_SIZE; ++i) {
        if(size < fixedSizePools[i].getObjectSize()) {
            return fixedSizePools[i].allocateUnsafe();
        }
    }

    void* ptr = nullptr;
    {
        for(auto& chunk : bigChunks) {
            if(chunk.removeDelay > 0.f && (chunk.size >= size && chunk.size < size * 2)) {
                chunk.removeDelay = -1.f;
                return chunk.ptr + sizeof(ChunkHeader);
            }
        }
        BigChunk chunk;
        chunk.size = size;
        chunk.ptr = new uint8_t [size + sizeof(ChunkHeader)];

        chunk.removeDelay = -1.f;
        *reinterpret_cast<ChunkHeader*>(chunk.ptr) = {0, CHUNKS_POOL_ID};
        bigChunks.push_back(chunk);

        ptr = chunk.ptr + sizeof(ChunkHeader);
    }

    return ptr;
}

void MainMemoryAllocator::deallocateUnsafe(void* ptr) {
    if(!ptr) {
        return;
    }
    auto poolId = PoolAllocator<Memory::AdaptiveGrowPolicy>::GetPoolId(ptr);
    if(poolId == CHUNKS_POOL_ID) {
        bool foundChunk = false;
        auto chunkPtr = static_cast<uint8_t*>(ptr) - sizeof(ChunkHeader);
        for(auto& chunk : bigChunks) {
            if(chunk.ptr == chunkPtr) {
                chunk.removeDelay = BIG_CHUNK_REMOVE_DELAY;
                foundChunk = true;
                break;
            }
        }
        assert(foundChunk && "Can't find big chunk to deallocate");
    } else {
        assert(poolId < MAX_FIXED_BLOCK_SIZE && "Invalid pool id");
        fixedSizePools[poolId].deallocateUnsafe(ptr);
    }
}

void MainMemoryAllocator::update(float dt) {
    memLock();

    bigChunks.erase(std::remove_if(bigChunks.begin(), bigChunks.end(), [dt](BigChunk& chunk){
        if(chunk.removeDelay < 0.f) {
            return false;
        }
        chunk.removeDelay -= dt;
        if(chunk.removeDelay <= 0.f) {
            delete[] chunk.ptr;
            return true;
        }
        return false;
    }), bigChunks.end());

    memUnlock();
}

void MainMemoryAllocator::memLock() {
    mutex.lock();
}

void MainMemoryAllocator::memUnlock() {
    mutex.unlock();
}

size_t MainMemoryAllocator::getNumAliveBigChunks() {
    size_t count = 0;
    {
        memLock();
        for(const auto& chunk : bigChunks) {
            count += chunk.removeDelay < 0.f ? 1u : 0u;
        }
        memUnlock();
    }
    return count;
}

MemLocker::MemLocker() {
    GetEnv()->GetMemoryAllocator()->memLock();
}

MemLocker::~MemLocker() {
    GetEnv()->GetMemoryAllocator()->memUnlock();
}

} // namespace Memory