#ifndef __ET_REFLECT_INTERFACES_HPP__
#define __ET_REFLECT_INTERFACES_HPP__

#include "Core/TypeId.hpp"

#include <memory>

class ClassInfo;
class JSONNode;

struct ETClassInfoManager {
    virtual ~ETClassInfoManager() = default;
    virtual ClassInfo* ET_findClassInfoByName(const char* className) = 0;
    virtual ClassInfo* ET_findClassInfoByTypeId(TypeId classTypeId) = 0;
    virtual bool ET_registerClassInfo(std::unique_ptr<ClassInfo>& classInfo) = 0;
    virtual void ET_reset() = 0;
    virtual int ET_getRegisteredClassCount() = 0;
    virtual void ET_makeReflectModel(JSONNode& node) = 0;
};

#endif /* __ET_REFLECT_INTERFACES_HPP__ */