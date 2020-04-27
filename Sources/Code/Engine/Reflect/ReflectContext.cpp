#include "Reflect/ReflectContext.hpp"
#include "Reflect/ClassInfo.hpp"
#include "Reflect/ETReflectInterfaces.hpp"
#include "Core/ETPrimitives.hpp"

#include <cassert>

ReflectContext::ReflectContext() :
    registeredClsInfo(nullptr) {
}

ReflectContext::~ReflectContext() {
}

bool ReflectContext::registerClassInfo() {
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

bool ReflectContext::reflectEmbedded(const std::function<void(ReflectContext&)>& reflectFunc) {
    reflectFunc(*this);
    return registerClassInfo();
}

ClassInfo* ReflectContext::getRegisteredClassInfo() {
    return registeredClsInfo;
}

