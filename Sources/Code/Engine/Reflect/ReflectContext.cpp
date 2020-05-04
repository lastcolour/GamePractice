#include "Reflect/ReflectContext.hpp"
#include "Reflect/ClassInfo.hpp"
#include "Reflect/ETReflectInterfaces.hpp"
#include "Core/ETPrimitives.hpp"
#include "ETApplicationInterfaces.hpp"

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

ClassInfo* ReflectContext::createClassInfo(const char* className, TypeId classTypeId) {
    const char* errStr = "[ReflectContext::createClassInfo] Can't create class info for '%s' (Error: %s)";
    if(!className || !className[0]) {
        LogError(errStr, className, "empty name");
        return nullptr;
    }
    ClassInfo* cInfo = nullptr;
    ET_SendEventReturn(cInfo, &ETClassInfoManager::ET_findClassInfoByName, className);
    if(cInfo) {
        LogError(errStr, className, "already registered by name");
        return nullptr;
    }
    if(classTypeId == InvalidTypeId) {
        LogError(errStr, className, "instances have invalid type id");
        return nullptr;
    }
    ET_SendEventReturn(cInfo, &ETClassInfoManager::ET_findClassInfoByTypeId, classTypeId);
    if(cInfo) {
        LogError(errStr, className, "already registered by typeId");
        return nullptr;
    }
    clsInfo.reset(new ClassInfo(className, classTypeId));
    return clsInfo.get();
}

EnumInfo* ReflectContext::createEnumInfo(const char* enumName, TypeId enumTypeId) {
    const char* errStr = "[ReflectContext::createEnumInfo] Can't create enum info for '%s' (Error: %s)";
    if(!enumName || !enumName[0]) {
        LogError(errStr, enumName, "empty name");
        return nullptr;
    }
    EnumInfo* enumInfo = nullptr;
    ET_SendEventReturn(enumInfo, &ETClassInfoManager::ET_findEnumInfoByName, enumName);
    if(enumInfo) {
        LogError(errStr, enumName, "already registered by name");
        return nullptr;
    }
    if(enumTypeId == InvalidTypeId) {
        LogError(errStr, enumName, "enum have invalid type id");
        return nullptr;
    }
    ET_SendEventReturn(enumInfo, &ETClassInfoManager::ET_findEnumInfoByTypeId, enumTypeId);
    if(enumInfo) {
        LogError(errStr, enumName, "already registered by typeId");
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