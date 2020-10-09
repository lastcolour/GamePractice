#include "Reflect/ReflectContext.hpp"
#include "Reflect/ClassInfo.hpp"
#include "Reflect/ETReflectInterfaces.hpp"
#include "Reflect/EnumInfo.hpp"
#include "Core/ETPrimitives.hpp"
#include "Core/ETLogger.hpp"

#include <cassert>

ReflectContext::ReflectContext() :
    registeredClsInfo(nullptr) {
}

ReflectContext::~ReflectContext() {
}

bool ReflectContext::registerInfos() {
    if(!clsInfo) {
        return false;
    }
    bool res = false;
    registeredClsInfo = clsInfo.get();
    ET_SendEventReturn(res, &ETClassInfoManager::ET_registerClassInfo, clsInfo);
    assert(clsInfo == nullptr && "Expeced to move ownership of class info to manager");
    if(!res) {
        registeredClsInfo = nullptr;
    }
    return res;
}

bool ReflectContext::registerEnums() {
    bool res = true;
    for(auto& enumInfo : enumInfos) {
        ET_SendEventReturn(res, &ETClassInfoManager::ET_registerEnumInfo, enumInfo);
        if(!res) {
            LogError("[ReflectContext::registerEnums] Can't register enum: %s", enumInfo->getName());
            res = false;
            break;
        }
    }
    enumInfos.clear();
    return res;
}

ClassInfo* ReflectContext::createClassInfo(const char* className, TypeId classTypeId) {
    const char* errStr = "[ReflectContext::createClassInfo] Can't create class info for '%s' (Error: %s)";
    if(!className || !className[0]) {
        LogError(errStr, className, "empty name");
        assert(false && "can't create class info");
        return nullptr;
    }
    ClassInfo* cInfo = nullptr;
    ET_SendEventReturn(cInfo, &ETClassInfoManager::ET_findClassInfoByName, className);
    if(cInfo) {
        if(classTypeId != cInfo->getIntanceTypeId()) {
            LogError(errStr, className, "other class was registered by this name");
            assert(false && "can't create class info");
        }
        return nullptr;
    }
    if(classTypeId == InvalidTypeId) {
        LogError(errStr, className, "instances have invalid type id");
        assert(false && "can't create class info");
        return nullptr;
    }
    ET_SendEventReturn(cInfo, &ETClassInfoManager::ET_findClassInfoByTypeId, classTypeId);
    if(cInfo) {
        std::string name = className;
        if(name != cInfo->getName()) {
            LogError(errStr, className, "class already registered by other name");
            assert(false && "can't create class info");
        }
        return nullptr;
    }
    if(!registerEnums()) {
        assert(false && "can't create class info");
        return nullptr;
    }
    clsInfo.reset(new ClassInfo(className, classTypeId));
    return clsInfo.get();
}

EnumInfo* ReflectContext::createEnumInfo(const char* enumName, TypeId enumTypeId) {
    const char* errStr = "[ReflectContext::createEnumInfo] Can't create enum info for '%s' (Error: %s)";
    if(!enumName || !enumName[0]) {
        LogError(errStr, enumName, "empty name");
        assert(false && "can't create enum info");
        return nullptr;
    }
    EnumInfo* enumInfo = nullptr;
    ET_SendEventReturn(enumInfo, &ETClassInfoManager::ET_findEnumInfoByName, enumName);
    if(enumInfo) {
        LogError(errStr, enumName, "already registered by name");
        assert(false && "can't create enum info");
        return nullptr;
    }
    if(enumTypeId == InvalidTypeId) {
        LogError(errStr, enumName, "enum have invalid type id");
        assert(false && "can't create enum info");
        return nullptr;
    }
    ET_SendEventReturn(enumInfo, &ETClassInfoManager::ET_findEnumInfoByTypeId, enumTypeId);
    if(enumInfo) {
        LogError(errStr, enumName, "already registered by typeId");
        assert(false && "can't create enum info");
        return nullptr;
    }
    enumInfos.push_back(std::unique_ptr<EnumInfo>(new EnumInfo(enumName, enumTypeId)));
    return enumInfos.back().get();
}

bool ReflectContext::reflectEmbedded(const std::function<void(ReflectContext&)>& reflectFunc) {
    reflectFunc(*this);
    return registerInfos();
}

ClassInfo* ReflectContext::getRegisteredClassInfo() {
    return registeredClsInfo;
}