#include "Reflect/ClassInfoManager.hpp"
#include "Reflect/EnumInfo.hpp"
#include "ArrayInfo.hpp"
#include "ResourceInfo.hpp"
#include "Core/JSONNode.hpp"

#include <cassert>

namespace Reflect {

ClassInfoManager::ClassInfoManager() {
}

ClassInfoManager::~ClassInfoManager() {
}

ClassInfo* ClassInfoManager::findClassInfoByName(const char* className) {
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

bool ClassInfoManager::registerClassInfo(ClassInfoPtrT& classInfo) {
    if(!classInfo) {
        LogError("[ClassInfoManager::registerClassInfo] Invalid class info");
        assert(false && "Invalid class info");
        return false;
    }
    if(findClassInfoByName(classInfo->getName())) {
        LogError("[ClassInfoManager::registerClassInfo] Class name duplicate: '%s'", classInfo->getName());
        assert(false && "Class name duplicate");
        return false;
    }
    if(findClassInfoByTypeId(classInfo->getIntanceTypeId())) {
        LogError("[ClassInfoManager::registerClassInfo] Class info already registered: '%s'", classInfo->getName());
        assert(false && "Class instance typeId duplicate");
        return false;
    }
    classInfoMap[classInfo->getIntanceTypeId()] = std::move(classInfo);
    return true;
}

ClassInfo* ClassInfoManager::findClassInfoByTypeId(Core::TypeId classTypeId) {
    auto it = classInfoMap.find(classTypeId);
    if(it != classInfoMap.end()) {
        return it->second.get();
    }
    return nullptr;
}

void ClassInfoManager::reset() {
    classInfoMap.clear();
    enumInfoMap.clear();
    arrayInfoMap.clear();
    resourceInfoMap.clear();
}

int ClassInfoManager::getRegisteredClassCount() {
    return static_cast<int>(classInfoMap.size());
}

void ClassInfoManager::makeReflectModel(JSONNode& node) {
    if(node) {
        assert(false && "JSONNode should be empty");
        return;
    }
    for(auto& enumInfoNode : enumInfoMap) {
        auto& eunmInfo = enumInfoNode.second;
        JSONNode enumNode;
        eunmInfo->makeReflectModel(enumNode);
        node.write(eunmInfo->getName(), enumNode);
    }
    for(auto& classInfoItem : classInfoMap) {
        auto& classInfo = classInfoItem.second;
        JSONNode classNode;
        classInfo->makeReflectModel(classNode);
        node.write(classInfo->getName(), classNode);
    }
    for(auto& resourceInfoItem : resourceInfoMap) {
        auto& resourceInfo = resourceInfoItem.second;
        JSONNode resourceNode;
        resourceInfo->makeReflectModel(resourceNode);
        node.write(resourceInfo->getName(), resourceNode);
    }
}

EnumInfo* ClassInfoManager::findEnumInfoByTypeId(Core::TypeId enumTypeId) {
    auto it = enumInfoMap.find(enumTypeId);
    if(it != enumInfoMap.end()) {
        return it->second.get();
    }
    return nullptr;
}

EnumInfo* ClassInfoManager::findEnumInfoByName(const char* enumName) {
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

bool ClassInfoManager::registerEnumInfo(std::unique_ptr<EnumInfo>& enumInfo) {
    if(!enumInfo) {
        LogError("[ClassInfoManager::registerEnumInfo] Invalid enum info");
        assert(false && "Invalid enum info");
        return false;
    }
    if(findEnumInfoByTypeId(enumInfo->getEnumTypeId())) {
        LogError("[ClassInfoManager::registerEnumInfo] Enum info already registered");
        assert(false && "Enum typeId duplicate");
        return false;
    }
    enumInfoMap[enumInfo->getEnumTypeId()] = std::move(enumInfo);
    return true;
}

ResourceInfo* ClassInfoManager::findResourceInfoByTypeId(Core::TypeId resourceSpecTypeId) {
    auto it = resourceInfoMap.find(resourceSpecTypeId);
    if(it == resourceInfoMap.end()) {
        return nullptr;
    }
    return it->second.get();
}

bool ClassInfoManager::registerResourceInfo(std::unique_ptr<ResourceInfo>& resourceInfo) {
    if(!resourceInfo) {
        LogError("[ClassInfoManager::registerResourceInfo] Invalid resource info");
        assert(false && "Invalid resource info");
        return false;
    }
    if(findResourceInfoByTypeId(resourceInfo->getSpecTypeId())) {
        LogError("[ClassInfoManager::registerResourceInfo] Resouce info already registered");
        assert(false && "Resource spec typeId duplicate");
        return false;
    }
    resourceInfoMap[resourceInfo->getSpecTypeId()] = std::move(resourceInfo);
    return true;
}

ArrayInfo* ClassInfoManager::findArrayInfoByElemTypeId(Core::TypeId elemTypeId) {
    auto it = arrayInfoMap.find(elemTypeId);
    if(it == arrayInfoMap.end()) {
        return nullptr;
    }
    return it->second.get();
}

bool ClassInfoManager::registerArrayInfo(std::unique_ptr<ArrayInfo>& arrayInfo) {
    if(!arrayInfo) {
        LogError("[ClassInfoManager::registerArrayInfo] Invalid array info");
        assert(false && "Invalid array info");
        return false;
    }
    if(findArrayInfoByElemTypeId(arrayInfo->getElemTypeId())) {
        LogError("[ClassInfoManager::registerArrayInfo] Array info already registered");
        assert(false && "Array elem typeId duplicate");
        return false;
    }
    arrayInfoMap[arrayInfo->getElemTypeId()] = std::move(arrayInfo);
    return true;
}

} // namespace Reflect