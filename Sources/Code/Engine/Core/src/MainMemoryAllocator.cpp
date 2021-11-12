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

} // namespace

namespace Memory {

MainMemoryAllocator::MainMemoryAllocator() {
    assert(sizeof(FIXED_POOL_SIZES) / sizeof(size_t) == MAX_FIXED_BLOCK_SIZE && "Invalid amount of pool sizes");
    for(int i = 0; i < MAX_FIXED_BLOCK_SIZE; ++i) {
        fixedSizePools[i].setPoolId(static_cast<BaseUintT>(i));
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
    std::lock_guard<std::mutex> lock(mutex);

    for(uint32_t i = 0; i < MAX_FIXED_BLOCK_SIZE; ++i) {
        if(size < fixedSizePools[i].getObjectSize()) {
            return fixedSizePools[i].allocate();
        }
    }
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

    return chunk.ptr + sizeof(ChunkHeader);
}

void MainMemoryAllocator::deallocate(void* ptr) {
    if(!ptr) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex);

    auto poolId = PoolAllocator<Memory::AdaptiveGrowPolicy>::GetPoolId(ptr);
    if(poolId == CHUNKS_POOL_ID) {
        auto chunkPtr = static_cast<uint8_t*>(ptr) - sizeof(ChunkHeader);
        for(auto& chunk : bigChunks) {
            if(chunk.ptr == chunkPtr) {
                chunk.removeDelay = BIG_CHUNK_REMOVE_DELAY;
                break;
            }
        }
    } else {
        assert(0 <= poolId && poolId < MAX_FIXED_BLOCK_SIZE && "Invalid pool id");
        fixedSizePools[poolId].deallocate(ptr);
    }
}

void MainMemoryAllocator::update(float dt) {
    std::lock_guard<std::mutex> lock(mutex);
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
}

} // namespace Memory