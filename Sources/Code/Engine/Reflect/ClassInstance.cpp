#include "Reflect/ClassInstance.hpp"
#include "Reflect/ClassInfo.hpp"
#include "ETApplicationInterfaces.hpp"

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

bool ClassInstance::readAllValuesFrom(const JSONNode& node) {
    return readValueFrom(AllEntityLogicValueId, node);
}

bool ClassInstance::readAllValuesFrom(MemoryStream& stream) {
    return readValueFrom(AllEntityLogicValueId, stream);
}

bool ClassInstance::writeAllValuesTo(MemoryStream& stream) {
    return writeValueTo(AllEntityLogicValueId, stream);
}

bool ClassInstance::readValueFrom(EntityLogicValueId valueId, const JSONNode& node) {
    if(!instance) {
        assert(false && "Invalid instance");
        return false;
    }
    if(!classInfo) {
        LogError("[ClassInstance::readValueFrom] Can't read value of instance without class info");
        return false;
    }
    return classInfo->readValueFrom(instance, valueId, node);
}

bool ClassInstance::readValueFrom(EntityLogicValueId valueId, MemoryStream& stream) {
    if(!instance) {
        assert(false && "Invalid instance");
        return false;
    }
    if(!classInfo) {
        LogError("[ClassInstance::readValueFrom] Can't read value of instance without class info");
        return false;
    }
    return classInfo->readValueFrom(instance, valueId, stream);
}

bool ClassInstance::writeValueTo(EntityLogicValueId valueId, MemoryStream& stream) {
    if(!instance) {
        assert(false && "Invalid instance");
        return false;
    }
    if(!classInfo) {
        LogError("[ClassInstance::writeValueTo] Can't write value of instance without class info");
        return false;
    }
    return classInfo->writeValueTo(instance, valueId, stream);
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