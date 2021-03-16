#include "Core/GlobalData.hpp"
#include "Core/ETAssets.hpp"

#include <cassert>

GlobalData::GlobalData() {
}

GlobalData::~GlobalData() {
}

void GlobalData::createAndLoad(ClassInfo* classInfo, const char* fileName) {
    if(!classInfo) {
        assert(false && "Invalid class info");
        return;
    }
    if(!fileName || !fileName[0]) {
        assert(false && "Invalid file name");
        return;
    }

    if(!ET_IsExistNode<ETAssets>()) {
        LogError("[GlobalData::createAndLoad] Can't load config from file '%s' because ETAssets node does not exist",
            fileName);
    }

    JSONNode node;
    ET_SendEventReturn(node, &ETAssets::ET_loadJSONAsset, fileName);
    if(!node) {
        return;
    }
}

void GlobalData::create(ClassInfo* classInfo) {
}

void* GlobalData::getByTypeId(TypeId typeId) {
    return nullptr;
}