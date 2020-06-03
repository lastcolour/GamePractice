#ifndef __CLASS_INSTANCE_HPP__
#define __CLASS_INSTANCE_HPP__

#include "Core/TypeId.hpp"
#include "Core/Buffer.hpp"

#include <functional>
#include <memory>

class ClassInfo;
class MemoryStream;

class ClassInstance {

    using DeleteFuncT = void(*)(void*);

public:

    template<typename T>
    static ClassInstance CreateWithoutClassInfo(T* ptr) {
        auto deleteFunc = [](void* objectPtr){
            delete static_cast<T*>(objectPtr);
        };
        ClassInstance instance;
        instance.setDeleteFuncAndPtr(static_cast<DeleteFuncT>(deleteFunc), ptr);
        return instance;
    }

public:

    ClassInstance();
    ClassInstance(ClassInfo& clsInfo, void* clsInstance);
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
    bool readValue(EntityLogicValueId valueId, MemoryStream& stream);
    bool writeValue(EntityLogicValueId valueId, MemoryStream& stream);

private:

    void setDeleteFuncAndPtr(DeleteFuncT deleteF, void* ptr);
    bool isInstanceOfType(TypeId typeId) const;

private:

    ClassInstance(const ClassInstance&) = delete;
    ClassInstance& operator=(const ClassInstance&) = delete;

private:

    ClassInfo* classInfo;
    void* instance;
    DeleteFuncT deleteFunc;
};

#endif /* __CLASS_INSTANCE_HPP__ */