#include "ModuleSharedManager.hpp"
#include "Reflect/ClassInstance.hpp"

#include <cassert>

ModuleSharedManager::ModuleSharedManager() {
}

ModuleSharedManager::~ModuleSharedManager() {
}

bool ModuleSharedManager::init() {
    ETNode<ETModuleSharedManager>::connect(getEntityId());
    return true;
}

void ModuleSharedManager::deinit() {
    sharedObjects.clear();
    ETNode<ETModuleSharedManager>::disconnect();
}

void ModuleSharedManager::ET_registerShared(ClassInstance& instance) {
    std::lock_guard<std::mutex> lock(mutex);
    if(!instance.get()) {
        assert(false && "Invalid config");
        return;
    }
    auto typeId = instance.getInstanceTypeId();
    auto it = sharedObjects.find(typeId);
    if(it != sharedObjects.end()) {
        assert(false && "double registration");
        return;
    }
    sharedObjects[typeId] = std::move(instance);
}

const void* ModuleSharedManager::ET_getShared(TypeId typeId) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = sharedObjects.find(typeId);
    if(it != sharedObjects.end()) {
        return it->second.get();
    }
    assert(false && "shared is not found");
    return nullptr;
}