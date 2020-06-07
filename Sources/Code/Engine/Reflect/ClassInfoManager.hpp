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
    using EnumInfoPtrT = std::unique_ptr<EnumInfo>;
    using ArrayInfoPtrT = std::unique_ptr<ArrayInfo>;

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
    EnumInfo* ET_findEnumInfoByTypeId(TypeId enumTypeId) override;
    EnumInfo* ET_findEnumInfoByName(const char* enumName) override;
    bool ET_registerEnumInfo(std::unique_ptr<EnumInfo>& enumInfo) override;
    ArrayInfo* ET_findArrayInfoByElemTypeId(TypeId elemTypeId) override;
    bool ET_registerArrayInfo(std::unique_ptr<ArrayInfo>& arrayInfo) override;
    void ET_reset() override;
    int ET_getRegisteredClassCount() override;
    void ET_makeReflectModel(JSONNode& node) override;

private:

    std::unordered_map<TypeId, ClassInfoPtrT> classInfoMap;
    std::unordered_map<TypeId, EnumInfoPtrT> enumInfoMap;
    std::unordered_map<TypeId, ArrayInfoPtrT> arrayInfoMap;
};

#endif /* __CLASS_INFO_MANAGER_HPP__ */