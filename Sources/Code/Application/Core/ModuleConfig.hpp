#ifndef __MODULE_CONFIG_HPP__
#define __MODULE_CONFIG_HPP__

#include "ETApplicationInterfaces.hpp"

class ModuleConfigBase {
public:
    virtual ~ModuleConfigBase() = default;
    virtual void serialize(const JSONNode& node) = 0;
    virtual void* getConfigPtr() = 0;
};

template<typename ConfigType>
class SystemModuleConfig : public ModuleConfigBase {
public:

    SystemModuleConfig() {
        auto configId = GetTypeId<ConfigType>();
        ET_SendEvent(&ETModuleConfigManager::ET_registerConfig, this, configId);
    }

    virtual ~SystemModuleConfig() {
        auto configId = GetTypeId<ConfigType>();
        ET_SendEvent(&ETModuleConfigManager::ET_removeConfig, configId);
    }

    // ModuleConfigBase
    void serialize(const JSONNode& node) override {
        config.serialize(node);
    }

    void* getConfigPtr() override {
        return &config;
    }

private:

    ConfigType config;
};

#endif /* __MODULE_CONFIG_HPP__ */