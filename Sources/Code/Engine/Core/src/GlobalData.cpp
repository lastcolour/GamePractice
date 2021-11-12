#include "Core/ETAssets.hpp"

#include <cassert>

namespace Core {

GlobalData::GlobalData() {
}

GlobalData::~GlobalData() {
}

void GlobalData::createAndLoad(Reflect::ClassInfo* classInfo, const char* fileName) {
    if(!classInfo) {
        assert(false && "Invalid class info");
        return;
    }
    if(!fileName || !fileName[0]) {
        assert(false && "Invalid file name");
        return;
    }
    if(!ET_IsExistNode<ETAssets>()) {
        assert(false && "ETAssets node doesn't exist");
        return;
    }

    auto typeId = classInfo->getIntanceTypeId();
    if(data.count(typeId)) {
        assert(false && "Instance with the same type id already registered");
        return;
    }

    auto instance = classInfo->createInstance();
    if(!instance.get()) {
        LogError("[GlobalData::createAndLoad] Can't create instance of '%s'", classInfo->getName());
        return;
    }

    if(!Reflect::LoadObjectFromAsset(instance, fileName)) {
        LogError("[GlobalData::createAndLoad] Can't read instance data of instance '%s' from: '%s'",
            classInfo->getName(), fileName);
    }

    data[typeId] = std::move(instance);
}

void GlobalData::create(Reflect::ClassInfo* classInfo) {
    if(!classInfo) {
        assert(false && "Invalid class info");
        return;
    }

    auto typeId = classInfo->getIntanceTypeId();
    if(data.count(typeId)) {
        assert(false && "Instance with the same type id already registered");
        return;
    }

    auto instance = classInfo->createInstance();
    if(!instance.get()) {
        LogError("[GlobalData::create] Can't create instance of '%s'",
            classInfo->getName());
        return;
    }

    data[typeId] = std::move(instance);
}

void* GlobalData::getByTypeId(TypeId typeId) {
    auto it = data.find(typeId);
    if(it != data.end()) {
        return it->second.get();
    }
    return nullptr;
}

void GlobalData::removeByTypeId(TypeId typeId) {
    auto it = data.find(typeId);
    if(it != data.end()) {
        data.erase(it);
    } else {
        LogWarning("[GlobalData::removeByTypeId] Can't find data with type id '%d' to remove",
            typeId);
    }
}

} // namespace Core