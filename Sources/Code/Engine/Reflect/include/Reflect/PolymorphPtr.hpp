#ifndef __POLYMORPH_PTR_HPP__
#define __POLYMORPH_PTR_HPP__

#include "Reflect/ClassInstance.hpp"

namespace Reflect {

template<typename T>
class PolymorphPtr {
public:

    using ObjectType = T;

public:

    PolymorphPtr() {}

    PolymorphPtr(PolymorphPtr&& other) :
        object(std::move(other.object)) {
    }

    PolymorphPtr& operator=(PolymorphPtr&& other) {
        if(this != &other) {
            object = std::move(other.object);
        }
        return *this;
    }

    ~PolymorphPtr() {}

    T* operator->() {
        return get();
    }

    const T* operator->() const {
        return get();
    }

    T* get() {
        return static_cast<T*>(object.get());
    }

    const T* get() const {
        return static_cast<const T*>(object.get());
    }

    ClassInstance& getInstance() {
        return object;
    }

    const ClassInstance& getInstance() const {
        return object;
    }

private:

    ClassInstance object;
};

} // namespace Reflect

#endif /* __POLYMORPH_PTR_HPP__ */