#include "Core/ModuleConfigManager.hpp"
#include "Reflect/ClassInstance.hpp"

#include <cassert>

ModuleConfigManager::ModuleConfigManager() {
}

ModuleConfigManager::~ModuleConfigManager() {
}

bool ModuleConfigManager::init() {
    ETNode<ETModuleConfigManager>::connect(getEntityId());
    return true;
}

void ModuleConfigManager::deinit() {
    configs.clear();
    ETNode<ETModuleConfigManager>::disconnect();
}

void ModuleConfigManager::ET_registerConfig(ClassInstance& configInstance) {
    if(!configInstance.get()) {
        assert(false && "Invalid config");
        return;
    }
    auto configId = configInstance.getInstanceTypeId();
    auto it = configs.find(configId);
    if(it != configs.end()) {
        assert(false && "Double config registration");
        return;
    }
    configs[configId] = std::move(configInstance);
}

void* ModuleConfigManager::ET_getConfig(TypeId configId) {
    auto it = configs.find(configId);
    if(it != configs.end()) {
        return it->second.get();
    }
    assert(false && "Configs is not registered");
    return nullptr;
}