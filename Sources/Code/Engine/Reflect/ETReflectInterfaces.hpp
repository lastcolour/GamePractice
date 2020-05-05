#ifndef __ET_REFLECT_INTERFACES_HPP__
#define __ET_REFLECT_INTERFACES_HPP__

#include "Core/TypeId.hpp"

#include <memory>

class ClassInfo;
class EnumInfo;
class ArrayInfo;
class JSONNode;

struct ETClassInfoManager {
    virtual ~ETClassInfoManager() = default;
    virtual ClassInfo* ET_findClassInfoByName(const char* className) = 0;
    virtual ClassInfo* ET_findClassInfoByTypeId(TypeId classTypeId) = 0;
    virtual bool ET_registerClassInfo(std::unique_ptr<ClassInfo>& classInfo) = 0;
    virtual EnumInfo* ET_findEnumInfoByTypeId(TypeId enumTypeId) = 0;
    virtual EnumInfo* ET_findEnumInfoByName(const char* enumName) = 0;
    virtual bool ET_registerEnumInfo(std::unique_ptr<EnumInfo>& enumInfo) = 0;
    virtual ArrayInfo* ET_findArrayInfoByElemTypeId(TypeId elemTypeId) = 0;
    virtual bool ET_registerArrayInfo(std::unique_ptr<ArrayInfo>& arrayInfo) = 0;
    virtual void ET_reset() = 0;
    virtual int ET_getRegisteredClassCount() = 0;
    virtual void ET_makeReflectModel(JSONNode& node) = 0;
};

#endif /* __ET_REFLECT_INTERFACES_HPP__ */