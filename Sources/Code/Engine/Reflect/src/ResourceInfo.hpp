#ifndef __RESOURCE_INFO_HPP__
#define __RESOURCE_INFO_HPP__

#include "Core/JSONNode.hpp"
#include "Reflect/Resource.hpp"

namespace Reflect {

class ResourceInfo {
public:

    using ConvertFuncT = Impl::ResourceConvertFuncT;
    using TempBuffInitFuncT = Impl::ResourceTempBuffInitFuncT;

public:

    ResourceInfo(Core::TypeId specTypeId, ClassValueType elemType, Core::TypeId elemTypeId, const char* resourceName,
        ConvertFuncT toRuntimeFunc, ConvertFuncT toStorageFunc, TempBuffInitFuncT buffInitFunc);
    ~ResourceInfo();

    Core::TypeId getSpecTypeId() const;
    const char* getName() const;
    void makeReflectModel(JSONNode& node);
    bool writeValuesTo(const SerializeContext& ctx, void* valuePtr, ClassValue& targetValue, JSONNode& node);
    bool writeValuesTo(const SerializeContext& ctx, void* valuePtr, ClassValue& targetValue, Memory::MemoryStream& stream);
    bool readValuesFrom(const SerializeContext& ctx, void* valuePtr, ClassValue& targetValue, const JSONNode& node);
    bool readValuesFrom(const SerializeContext& ctx, void* valuePtr, ClassValue& targetValue, Memory::MemoryStream& stream);

private:

    Core::TypeId specTypeId;
    ClassValue resourceValue;
    ConvertFuncT toRuntime;
    ConvertFuncT toStorage;
    TempBuffInitFuncT buffInit;
    std::string name;
};

} // namespace Reflect

#endif /* __RESOURCE_INFO_HPP__ */