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
        return resPtr;
    }

private:

    bool isInstanceOfType(TypeId typeId) const;

private:

    const ClassInfo* classInfo;
    void* instance;
};
#endif /* __CLASS_INSTANCE_HPP__ */