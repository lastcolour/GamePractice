#include "Core/PoolAllocator.hpp"

#include <cassert>
#include <iostream>

PoolAllocator::PoolAllocator(size_t objectSize, size_t objectsPerBlock) :
    objSize(objectSize),
    objPerBlock(objectsPerBlock) {
}

PoolAllocator::~PoolAllocator() {
}

void* PoolAllocator::allocate() {
    BlockHeader* block = nullptr;
    for(auto& b : blocks) {
        auto header = reinterpret_cast<BlockHeader*>(b.get());
        if(header->free) {
            block = header;
            break;
        }
    }
    if(!block) {
        block = allocateBlock();
        blocks.emplace_back(reinterpret_cast<unsigned char*>(block));
    }
    auto freeObj = static_cast<ObjectPtr*>(block->free);
    block->free = freeObj->next;
    return freeObj;
}

void PoolAllocator::deallocate(void* ptr) {
    for(auto& block : blocks) {
        auto start = block.get() + sizeof(BlockHeader);
        auto end = start + objSize * objPerBlock;
        if(ptr >= start && ptr < end) {
            auto frac = (reinterpret_cast<uintptr_t>(ptr) - reinterpret_cast<uintptr_t>(start)) % objSize;
            if(frac != 0) {
                assert(false && "Invalid ptr");
                return;
            }
            auto header = reinterpret_cast<BlockHeader*>(block.get());
            auto objPtr = static_cast<ObjectPtr*>(ptr);
            objPtr->next = header->free;
            header->free = objPtr;
            return;
        }
    }
}

PoolAllocator::BlockHeader* PoolAllocator::allocateBlock() {
    size_t memSize = objSize * objPerBlock + sizeof(BlockHeader);
    auto ptr = new unsigned char [memSize];
    if(!ptr) {
        assert(false && "Can't allocate memory block");
        return nullptr;
    }

    auto header = reinterpret_cast<BlockHeader*>(ptr);
    header->free = ptr + sizeof(BlockHeader);

    ObjectPtr* currObjPtr = static_cast<ObjectPtr*>(header->free);
    for(size_t i = 0; i < (objPerBlock - 1); ++i) {
        currObjPtr->next = reinterpret_cast<unsigned char*>(currObjPtr) + objSize;
        currObjPtr = reinterpret_cast<ObjectPtr*>(currObjPtr->next);
    }
    currObjPtr->next = nullptr;

    return header;
}