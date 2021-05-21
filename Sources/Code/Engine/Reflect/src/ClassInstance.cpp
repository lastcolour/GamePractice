#include "Reflect/ClassInstance.hpp"
#include "Reflect/ClassInfo.hpp"

#include <cassert>

ClassInstance::ClassInstance() :
    classInfo(nullptr),
    instance(nullptr),
    deleteFunc(nullptr) {
}

ClassInstance::ClassInstance(ClassInstance&& other) :
    classInfo(other.classInfo),
    instance(other.instance),
    deleteFunc(other.deleteFunc) {

    other.instance = nullptr;
    other.classInfo = nullptr;
    other.deleteFunc = nullptr;
}

ClassInstance& ClassInstance::operator=(ClassInstance&& other) {
    if(this == &other) {
        return *this;
    }
    if(instance) {
        deleteFunc(instance);
    }
    instance = other.instance;
    classInfo = other.classInfo;
    deleteFunc = other.deleteFunc;
    other.instance = nullptr;
    other.classInfo = nullptr;
    other.deleteFunc = nullptr;
    return *this;
}

ClassInstance::ClassInstance(ClassInfo& clsInfo, void* clsInstance) :
    classInfo(&clsInfo),
    instance(clsInstance),
    deleteFunc(clsInfo.getDeleteFunction()) {
}

ClassInstance::~ClassInstance() {
    if(instance) {
        deleteFunc(instance);
    }
    instance = nullptr;
}

bool ClassInstance::isInstanceOfType(TypeId typeId) const {
    if(!classInfo) {
        return false;
    }
    return classInfo->getIntanceTypeId() == typeId;
}

ClassInfo* ClassInstance::getClassInfo() {
    return classInfo;
}

TypeId ClassInstance::getInstanceTypeId() const {
    if(!classInfo) {
        return InvalidTypeId;
    }
    return classInfo->getIntanceTypeId();
}

void* ClassInstance::get() {
    return instance;
}

void ClassInstance::setDeleteFuncAndPtr(DeleteFuncT deleteF, void* ptr) {
    instance = ptr;
    deleteFunc = deleteF;
}

bool ClassInstance::readAllValuesFrom(const SerializeContext& ctx, const JSONNode& node) {
    return readValueFrom(ctx, AllEntityLogicValueId, node);
}

bool ClassInstance::readAllValuesFrom(const SerializeContext& ctx, MemoryStream& stream) {
    return readValueFrom(ctx, AllEntityLogicValueId, stream);
}

bool ClassInstance::writeAllValuesTo(const SerializeContext& ctx, JSONNode& node) {
    return writeValueTo(ctx, AllEntityLogicValueId, node);
}

bool ClassInstance::writeAllValuesTo(const SerializeContext& ctx, MemoryStream& stream) {
    return writeValueTo(ctx, AllEntityLogicValueId, stream);
}

bool ClassInstance::readValueFrom(const SerializeContext& ctx, EntityLogicValueId valueId, const JSONNode& node) {
    if(!instance) {
        assert(false && "Invalid instance");
        return false;
    }
    if(!classInfo) {
        LogError("[ClassInstance::readValueFrom] Can't read value of instance without class info");
        return false;
    }
    return classInfo->readValueFrom(ctx, instance, valueId, node);
}

bool ClassInstance::readValueFrom(const SerializeContext& ctx, EntityLogicValueId valueId, MemoryStream& stream) {
    if(!instance) {
        assert(false && "Invalid instance");
        return false;
    }
    if(!classInfo) {
        LogError("[ClassInstance::readValueFrom] Can't read value of instance without class info");
        return false;
    }
    return classInfo->readValueFrom(ctx, instance, valueId, stream);
}

bool ClassInstance::writeValueTo(const SerializeContext& ctx, EntityLogicValueId valueId, JSONNode& node) {
    if(!instance) {
        assert(false && "Invalid instance");
        return false;
    }
    if(!classInfo) {
        LogError("[ClassInstance::writeValueTo] Can't write value of instance without class info");
        return false;
    }
    return classInfo->writeValueTo(ctx, instance, valueId, node);
}

bool ClassInstance::writeValueTo(const SerializeContext& ctx, EntityLogicValueId valueId, MemoryStream& stream) {
    if(!instance) {
        assert(false && "Invalid instance");
        return false;
    }
    if(!classInfo) {
        LogError("[ClassInstance::writeValueTo] Can't write value of instance without class info");
        return false;
    }
    return classInfo->writeValueTo(ctx, instance, valueId, stream);
}

bool ClassInstance::addValueArrayElement(EntityLogicValueId valueId) {
    if(!instance) {
        assert(false && "Invalid instance");
        return false;
    }
    if(!classInfo) {
        LogError("[ClassInstance::addValueArrayElement] Can't add array element value to instance without class info");
        return false;
    }
    return classInfo->addNewValueArrayElement(instance, valueId);
}