#ifndef __OBJECT_POOL_HPP__
#define __OBJECT_POOL_HPP__

#include "Core/PoolAllocator.hpp"

namespace Memory {

namespace Impl {

class BaseObjectPool {
public:

    BaseObjectPool() = default;
    virtual ~BaseObjectPool() = default;

    virtual void* simpleCreate() = 0;
    virtual void recycle(void* ptr) = 0;
};

} // namespace Impl

template<typename T>
class ObjectPool : public Impl::BaseObjectPool {
public:

    ObjectPool() :
        allocator(sizeof(T), alignof(T)) {

        static_assert(alignof(T) <= 8, "Invalid Aligment");
    }

    ~ObjectPool() {
        allocator.visitEachObject([](void* ptr){
            reinterpret_cast<T*>(ptr)->~T();
        });
    }

    void* simpleCreate() override {
        if constexpr (std::is_default_constructible<T>::value) {
            return create();
        } else {
            return nullptr;
        }
    }

    template<typename ... ArgsT>
    T* create(ArgsT&& ... args) {
        return new (allocator.allocate()) T(std::forward<ArgsT>(args)...);
    }

    void recycle(void* ptr) override {
        reinterpret_cast<T*>(ptr)->~T();
        allocator.deallocate(ptr);
    }

private:

    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;

private:

    PoolAllocator<Memory::AdaptiveGrowPolicy> allocator;
};

} // namespace Memory

#endif /* __OBJECT_POOL_HPP__ */