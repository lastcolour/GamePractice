#ifndef __CLASS_INSTANCE_HPP__
#define __CLASS_INSTANCE_HPP__

#include <functional>
#include <memory>
#include "Core/TypeId.hpp"

class ClassInfo;

class ClassInstance {
public:

    ClassInstance();
    ClassInstance(const ClassInfo& clsInfo, void* clsInstance);
    ClassInstance(ClassInstance&& other);
    ClassInstance& operator=(ClassInstance&& other);
    ~ClassInstance();

    template<typename T>
    std::unique_ptr<T> acquire() {
        if(!instance) {
            return nullptr;
        }
        auto typeId = GetTypeId<T>();
        if(!isInstanceOfType(typeId)) {
            return nullptr;
        }
        auto resPtr = std::unique_ptr<T>(static_cast<T*>(instance));
        instance = nullptr;
        classInfo = nullptr;
        return resPtr;
    }

    void* get();

    TypeId getInstanceTypeId() const;

private:

    bool isInstanceOfType(TypeId typeId) const;

private:

    ClassInstance(const ClassInstance&) = delete;
    ClassInstance& operator=(const ClassInstance&) = delete;

private:

    const ClassInfo* classInfo;
    void* instance;
};
#endif /* __CLASS_INSTANCE_HPP__ */