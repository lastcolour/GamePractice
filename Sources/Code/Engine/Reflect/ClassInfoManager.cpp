#include "Reflect/ClassInfoManager.hpp"
#include "Reflect/ClassInfo.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"

#include <cassert>

ClassInfoManager::ClassInfoManager() {
}

ClassInfoManager::~ClassInfoManager() {
}

bool ClassInfoManager::init() {
    ETNode<ETClassInfoManager>::connect(getEntityId());
    return true;
}

void ClassInfoManager::deinit() {
}

ClassInfo* ClassInfoManager::ET_findClassInfoByName(const char* className) {
    std::string name = className;
    if(name.empty()) {
        return nullptr;
    }
    for(auto& classInfoNode : classInfoMap) {
        if(name == classInfoNode.second->getName()) {
            return classInfoNode.second.get();
        }
    }
    return nullptr;
}

bool ClassInfoManager::ET_registerClassInfo(ClassInfoPtrT& classInfo) {
    if(!classInfo) {
        assert(false && "Invalid class info");
        LogError("[ClassInfoManager::ET_registerClassInfo] Invalid class info");
        return false;
    }
    if(ET_findClassInfoByName(classInfo->getName())) {
        assert(false && "Class name duplicate");
        LogError("[ClassInfoManager::ET_registerClassInfo] Class name duplicate: %s", classInfo->getName());
        return false;
    }
    if(ET_findClassInfoByTypeId(classInfo->getIntanceTypeId())) {
        assert(false && "Class instance typeId duplicate");
        LogError("[ClassInfoManager::ET_registerClassInfo] Class instance typeId duplicate");
        return false;
    }
    classInfoMap[classInfo->getIntanceTypeId()] = std::move(classInfo);
    return true;
}

ClassInfo* ClassInfoManager::ET_findClassInfoByTypeId(TypeId classTypeId) {
    auto it = classInfoMap.find(classTypeId);
    if(it != classInfoMap.end()) {
        return it->second.get();
    }
    return nullptr;
}

void ClassInfoManager::ET_reset() {
    classInfoMap.clear();
}

int ClassInfoManager::ET_getRegisteredClassCount() {
    return static_cast<int>(classInfoMap.size());
}

void ClassInfoManager::ET_makeReflectModel(JSONNode& node) {
    if(node) {
        assert(false && "JSONNode should be empty");
        return;
    }
    for(auto& classInfoItem : classInfoMap) {
        auto& classInfo = classInfoItem.second;
        JSONNode classNode;
        classInfo->makeReflectModel(classNode);
        node.write(classInfo->getName(), classNode);
    }
}