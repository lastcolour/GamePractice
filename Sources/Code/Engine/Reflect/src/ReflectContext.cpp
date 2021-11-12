#include "Reflect/ClassInfoManager.hpp"
#include "Reflect/EnumInfo.hpp"

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
    registeredClsInfo = clsInfo.get();
    if(!GetEnv()->GetClassInfoManager()->registerClassInfo(clsInfo)) {
        registeredClsInfo = nullptr;
        return false;
    }
    assert(!clsInfo && "Expeced to move ownership of class info to manager");
    return true;
}

bool ReflectContext::registerEnums() {
    bool res = true;
    for(auto& enumInfo : enumInfos) {
        res = GetEnv()->GetClassInfoManager()->registerEnumInfo(enumInfo);
        if(!res) {
            LogError("[ReflectContext::registerEnums] Can't register enum: %s", enumInfo->getName());
            res = false;
            break;
        }
    }
    enumInfos.clear();
    return res;
}

Reflect::ClassInfo* ReflectContext::createClassInfo(const char* className, Core::TypeId classTypeId) {
    const char* errStr = "[ReflectContext::createClassInfo] Can't create class info for '%s' (Error: %s)";
    if(!className || !className[0]) {
        LogError(errStr, className, "empty name");
        assert(false && "can't create class info");
        return nullptr;
    }
    auto cInfo = GetEnv()->GetClassInfoManager()->findClassInfoByName(className);
    if(cInfo) {
        if(classTypeId != cInfo->getIntanceTypeId()) {
            LogError(errStr, className, "other class was registered by this name");
            assert(false && "can't create class info");
        }
        return nullptr;
    }
    if(classTypeId == Core::InvalidTypeId) {
        LogError(errStr, className, "instances have invalid type id");
        assert(false && "can't create class info");
        return nullptr;
    }
    cInfo = GetEnv()->GetClassInfoManager()->findClassInfoByTypeId(classTypeId);
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
    clsInfo.reset(new Reflect::ClassInfo(className, classTypeId));
    return clsInfo.get();
}

Reflect::EnumInfo* ReflectContext::createEnumInfo(const char* enumName, Core::TypeId enumTypeId) {
    const char* errStr = "[ReflectContext::createEnumInfo] Can't create enum info for '%s' (Error: %s)";
    if(!enumName || !enumName[0]) {
        LogError(errStr, enumName, "empty name");
        assert(false && "can't create enum info");
        return nullptr;
    }
    auto enumInfo = GetEnv()->GetClassInfoManager()->findEnumInfoByName(enumName);
    if(enumInfo) {
        LogError(errStr, enumName, "already registered by name");
        assert(false && "can't create enum info");
        return nullptr;
    }
    if(enumTypeId == Core::InvalidTypeId) {
        LogError(errStr, enumName, "enum have invalid type id");
        assert(false && "can't create enum info");
        return nullptr;
    }
    enumInfo = GetEnv()->GetClassInfoManager()->findEnumInfoByTypeId(enumTypeId);
    if(enumInfo) {
        LogError(errStr, enumName, "already registered by typeId");
        assert(false && "can't create enum info");
        return nullptr;
    }
    enumInfos.push_back(std::unique_ptr<Reflect::EnumInfo>(new Reflect::EnumInfo(enumName, enumTypeId)));
    return enumInfos.back().get();
}

bool ReflectContext::reflectEmbedded(const std::function<void(ReflectContext&)>& reflectFunc) {
    reflectFunc(*this);
    return registerInfos();
}

Reflect::ClassInfo* ReflectContext::getRegisteredClassInfo() {
    return registeredClsInfo;
}