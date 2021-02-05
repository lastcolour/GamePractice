#ifndef __ET_APLLICATION_HPP__
#define __ET_APLLICATION_HPP__

#include "Core/ETPrimitives.hpp"
#include "Core/TypeId.hpp"

class ClassInstance;

struct ETAppRunStateEvents {
    virtual ~ETAppRunStateEvents() = default;
    virtual void ET_onTerminate() = 0;
    virtual void ET_onPause() = 0;
    virtual void ET_onResume() = 0;
};

struct ETModuleSharedManager {
    virtual ~ETModuleSharedManager() = default;
    virtual void ET_registerShared(ClassInstance& configInstance) = 0;
    virtual const void* ET_getShared(TypeId configId) = 0;
};

template<typename SharedT>
const SharedT* ET_getShared() {
    const void* sharedObject = nullptr;
    auto sharedTypeId = GetTypeId<SharedT>();
    ET_SendEventReturn(sharedObject, &ETModuleSharedManager::ET_getShared, sharedTypeId);
    return reinterpret_cast<const SharedT*>(sharedObject);
}

#endif /* __ET_APLLICATION_HPP__ */