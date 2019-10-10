#include "Core/ModuleConfigManager.hpp"
#include "Core/ModuleConfig.hpp"

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
    ETNode<ETModuleConfigManager>::disconnect();
}

void ModuleConfigManager::ET_registerConfig(ModuleConfigBase* config, TypeId configId) {
    if(!config) {
        assert(false && "Invalid config");
        return;
    }
    auto it = configs.find(configId);
    if(it != configs.end()) {
        assert(false && "Double config registration");
        return;
    }
    configs[configId] = config;
}

void ModuleConfigManager::ET_removeConfig(TypeId configId) {
    auto it = configs.find(configId);
    if(it == configs.end()) {
        assert(false && "Remove invalid config");
        return;
    }
    configs.erase(it);
}

void* ModuleConfigManager::ET_getConfig(TypeId configId) {
    auto it = configs.find(configId);
    if(it != configs.end()) {
        return it->second->getConfigPtr();
    }
    return nullptr;
}