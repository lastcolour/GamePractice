#ifndef __ET_APLLICATION_HPP__
#define __ET_APLLICATION_HPP__

#include "Core/ETPrimitives.hpp"

class ClassInstance;

struct ETAppRunStateEvents {
    virtual ~ETAppRunStateEvents() = default;
    virtual void ET_onTerminate() = 0;
    virtual void ET_onPause() = 0;
    virtual void ET_onResume() = 0;
    virtual bool ET_isNeedRun() const = 0;
};


struct ETModuleConfigManager {
    virtual ~ETModuleConfigManager() = default;
    virtual void ET_registerConfig(ClassInstance& configInstance) = 0;
    virtual void* ET_getConfig(TypeId configId) = 0;
};

template<typename ConfigT>
ConfigT* ET_getConfig() {
    auto configId = GetTypeId<ConfigT>();
    void* moduleConfig = nullptr;
    ET_SendEventReturn(moduleConfig, &ETModuleConfigManager::ET_getConfig, configId);
    return reinterpret_cast<ConfigT*>(moduleConfig);
}

#endif /* __ET_APLLICATION_HPP__ */