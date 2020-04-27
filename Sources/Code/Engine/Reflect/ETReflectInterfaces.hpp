#ifndef __ET_REFLECT_INTERFACES_HPP__
#define __ET_REFLECT_INTERFACES_HPP__

#include "Core/TypeId.hpp"

#include <memory>

class ClassInfo;

struct ETClassInfoManager {
    virtual ~ETClassInfoManager() = default;
    virtual ClassInfo* ET_findClassInfoByName(const char* className) = 0;
    virtual ClassInfo* ET_findClassInfoByTypeId(TypeId classTypeId) = 0;
    virtual bool ET_registerClassInfo(std::unique_ptr<ClassInfo>& classInfo) = 0;
};

#endif /* __ET_REFLECT_INTERFACES_HPP__ */