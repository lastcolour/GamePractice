#include "ResourceInfo.hpp"
#include "Core/JSONNode.hpp"

#include <cassert>

namespace Reflect {

namespace {

ClassValue mergeValues(ClassValue& resourceValue, ClassValue& targetValue) {
    ClassValue newValue;
    newValue.isElement = targetValue.isElement;
    newValue.name = targetValue.name;
    newValue.typeId = resourceValue.typeId;
    newValue.type = resourceValue.type;
    return newValue;
}

} // namespace

ResourceInfo::ResourceInfo(Core::TypeId resourceSpecTypeId, ClassValueType elemType, Core::TypeId elemTypeId, const char* resourceName,
    ConvertFuncT toRuntimeFunc, ConvertFuncT toStorageFunc, TempBuffInitFuncT buffInitFunc) :
    specTypeId(resourceSpecTypeId),
    toRuntime(toRuntimeFunc),
    toStorage(toStorageFunc),
    buffInit(buffInitFunc) {

    resourceValue.type = elemType;
    resourceValue.typeId = elemTypeId;

    assert(resourceName && resourceName[0] && "Invalid resource name");

    name = StringFormat("resource.%s", resourceName);
}

ResourceInfo::~ResourceInfo() {
}

Core::TypeId ResourceInfo::getSpecTypeId() const {
    return specTypeId;
}

const char* ResourceInfo::getName() const {
    return name.c_str();
}

bool ResourceInfo::writeValuesTo(const SerializeContext& ctx, void* valuePtr, ClassValue& targetValue, JSONNode& node) {
    auto res = static_cast<Impl::BaseResource*>(valuePtr);
    auto data = res->getDataAccessor();

    auto mergedValue = mergeValues(resourceValue, targetValue);

    const bool isSet = *static_cast<bool*>(data.isSetFlag);
    if(!isSet) {
        if(!mergedValue.isElement) {
            node.writeNull(mergedValue.name.c_str());
        } else {
            node.writeNull();
        }
        return true;
    }

    if(toStorage) {
        u_char localStorage[Impl::BaseResource::MaxStorageTypeSize];
        buffInit(localStorage, true);
        toStorage(data.RuntimeData, localStorage);
        const bool retVal = mergedValue.writeValueTo(ctx, localStorage, node);
        buffInit(localStorage, false);
        return retVal;
    } else {
        return mergedValue.writeValueTo(ctx, data.RuntimeData, node);
    }
}

bool ResourceInfo::writeValuesTo(const SerializeContext& ctx, void* valuePtr, ClassValue& targetValue, Memory::MemoryStream& stream) {
    auto res = static_cast<Impl::BaseResource*>(valuePtr);
    auto data = res->getDataAccessor();

    auto mergedValue = mergeValues(resourceValue, targetValue);

    const bool isSet = *static_cast<bool*>(data.isSetFlag);

    stream.write(isSet);

    if(!isSet) {
        return true;
    }

    if(toStorage) {
        u_char localStorage[Impl::BaseResource::MaxStorageTypeSize];
        buffInit(localStorage, true);
        toStorage(data.RuntimeData, localStorage);
        bool retVal = mergedValue.writeValueTo(ctx, localStorage, stream);
        buffInit(localStorage, false);
        return retVal;
    } else {
        return mergedValue.writeValueTo(ctx, data.RuntimeData, stream);
    }
}

bool ResourceInfo::readValuesFrom(const SerializeContext& ctx, void* valuePtr, ClassValue& targetValue, const JSONNode& node) {
    auto res = static_cast<Impl::BaseResource*>(valuePtr);
    auto data = res->getDataAccessor();

    auto mergedValue = mergeValues(resourceValue, targetValue);

    if(!mergedValue.isElement) {
        if(node.isNull(mergedValue.name.c_str())) {
            return true;
        }
    } else {
        if(node.isNull()) {
            return true;
        }
    }

    bool retVal = false;

    if(toRuntime) {
        u_char localStorage[Impl::BaseResource::MaxStorageTypeSize];
        buffInit(localStorage, true);
        if(mergedValue.readValueFrom(ctx, localStorage, node)) {
            toRuntime(localStorage, data.RuntimeData);
            *static_cast<bool*>(data.isSetFlag) = true;
            retVal = true;
        }
        buffInit(localStorage, false);
    } else {
        if(mergedValue.readValueFrom(ctx, data.RuntimeData, node)) {
            *static_cast<bool*>(data.isSetFlag) = true;
            retVal = true;
        }
    }

    return retVal;
}

bool ResourceInfo::readValuesFrom(const SerializeContext& ctx, void* valuePtr, ClassValue& targetValue, Memory::MemoryStream& stream) {
    auto res = static_cast<Impl::BaseResource*>(valuePtr);
    auto data = res->getDataAccessor();

    auto mergedValue = mergeValues(resourceValue, targetValue);

    bool isSet = false;
    stream.read(isSet);

    if(!isSet) {
        return true;
    }

    bool retVal = false;

    if(toRuntime) {
        u_char localStorage[Impl::BaseResource::MaxStorageTypeSize];
        buffInit(localStorage, true);
        if(mergedValue.readValueFrom(ctx, localStorage, stream)) {
            toRuntime(localStorage, data.RuntimeData);
            *static_cast<bool*>(data.isSetFlag) = true;
            retVal = true;
        }
        buffInit(localStorage, false);
    } else {
        if(mergedValue.readValueFrom(ctx, data.RuntimeData, stream)) {
            *static_cast<bool*>(data.isSetFlag) = true;
            retVal = true;
        }
    }

    return retVal;
}

void ResourceInfo::makeReflectModel(JSONNode& node) {
    node.write("type", "resource");
    node.write("spec", name.c_str() + 9);
    node.write("storage", resourceValue.getTypeName());
}

} // namespace Reflect