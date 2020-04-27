#ifndef __CLASS_INFO_MANAGER_HPP__
#define __CLASS_INFO_MANAGER_HPP__

#include "Reflect/ETReflectInterfaces.hpp"
#include "Core/ETPrimitives.hpp"
#include "Core/SystemLogic.hpp"

#include <unordered_map>

class ClassInfoManager : SystemLogic,
    public ETNode<ETClassInfoManager> {
private:

    using ClassInfoPtrT = std::unique_ptr<ClassInfo>;

public:

    ClassInfoManager();
    virtual ~ClassInfoManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETClassInfoManager
    ClassInfo* ET_findClassInfoByName(const char* className) override;
    ClassInfo* ET_findClassInfoByTypeId(TypeId classTypeId) override;

    bool ET_registerClassInfo(ClassInfoPtrT& classInfo) override;

private:

    std::unordered_map<TypeId, ClassInfoPtrT> classInfoMap;
};

#endif /* __CLASS_INFO_MANAGER_HPP__ */