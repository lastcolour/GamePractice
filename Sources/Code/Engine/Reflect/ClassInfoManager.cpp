#include "Reflect/ClassInfoManager.hpp"
#include "Reflect/ClassInfo.hpp"
#include "Reflect/EnumInfo.hpp"
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
        LogError("[ClassInfoManager::ET_registerClassInfo] Invalid class info");
        assert(false && "Invalid class info");
        return false;
    }
    if(ET_findClassInfoByName(classInfo->getName())) {
        LogError("[ClassInfoManager::ET_registerClassInfo] Class name duplicate: %s", classInfo->getName());
        assert(false && "Class name duplicate");
        return false;
    }
    if(ET_findClassInfoByTypeId(classInfo->getIntanceTypeId())) {
        LogError("[ClassInfoManager::ET_registerClassInfo] Class info already registered");
        assert(false && "Class instance typeId duplicate");
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

EnumInfo* ClassInfoManager::ET_findEnumInfoByTypeId(TypeId enumTypeId) {
    auto it = enumInfoMap.find(enumTypeId);
    if(it != enumInfoMap.end()) {
        return it->second.get();
    }
    return nullptr;
}

EnumInfo* ClassInfoManager::ET_findEnumInfoByName(const char* enumName) {
    std::string name = enumName;
    if(name.empty()) {
        return nullptr;
    }
    for(auto& enumInfoNode : enumInfoMap) {
        if(name == enumInfoNode.second->getName()) {
            return enumInfoNode.second.get();
        }
    }
    return nullptr;
}

bool ClassInfoManager::ET_registerEnumInfo(std::unique_ptr<EnumInfo>& enumInfo) {
    if(!enumInfo) {
        LogError("[ClassInfoManager::ET_registerEnumInfo] Invalid enum info");
        assert(false && "Invalid enum info");
        return false;
    }
    if(ET_findEnumInfoByTypeId(enumInfo->getEnumTypeId())) {
        LogError("[ClassInfoManager::ET_registerEnumInfo] Enum info already registered");
        assert(false && "Enum typeId duplicate");
        return false;
    }
    enumInfoMap[enumInfo->getEnumTypeId()] = std::move(enumInfo);
    return true;
}