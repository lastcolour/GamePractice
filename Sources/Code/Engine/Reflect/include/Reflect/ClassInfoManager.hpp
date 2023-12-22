#ifndef __CLASS_INFO_MANAGER_HPP__
#define __CLASS_INFO_MANAGER_HPP__

namespace Reflect {

class ClassInfo;
class EnumInfo;
class ArrayInfo;
class ResourceInfo;

class ClassInfoManager {
private:

    using ClassInfoPtrT = std::unique_ptr<ClassInfo>;
    using EnumInfoPtrT = std::unique_ptr<EnumInfo>;
    using ArrayInfoPtrT = std::unique_ptr<ArrayInfo>;
    using ResourceInfoPtrT = std::unique_ptr<ResourceInfo>;

public:

    ClassInfoManager();
    ~ClassInfoManager();

    ClassInfo* findClassInfoByName(const char* className);
    ClassInfo* findClassInfoByTypeId(Core::TypeId classTypeId);
    bool registerClassInfo(ClassInfoPtrT& classInfo);

    EnumInfo* findEnumInfoByTypeId(Core::TypeId enumTypeId);
    EnumInfo* findEnumInfoByName(const char* enumName);
    bool registerEnumInfo(std::unique_ptr<EnumInfo>& enumInfo);

    ResourceInfo* findResourceInfoByTypeId(Core::TypeId resourceSpecTypeId);
    bool registerResourceInfo(std::unique_ptr<ResourceInfo>& resourceInfo);

    ArrayInfo* findArrayInfoByElemTypeId(Core::TypeId elemTypeId);
    bool registerArrayInfo(std::unique_ptr<ArrayInfo>& arrayInfo);

    void reset();
    int getRegisteredClassCount();
    void makeReflectModel(JSONNode& node);

private:

    std::unordered_map<Core::TypeId, ClassInfoPtrT> classInfoMap;
    std::unordered_map<Core::TypeId, EnumInfoPtrT> enumInfoMap;
    std::unordered_map<Core::TypeId, ArrayInfoPtrT> arrayInfoMap;
    std::unordered_map<Core::TypeId, ResourceInfoPtrT> resourceInfoMap;
};

} // namespace Reflect

#endif /* __CLASS_INFO_MANAGER_HPP__ */