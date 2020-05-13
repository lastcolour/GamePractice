#include "Reflect/ClassIntance.hpp"
#include "Reflect/ClassInfo.hpp"

ClassInstance::ClassInstance() :
    classInfo(nullptr),
    instance(nullptr) {
}

ClassInstance::ClassInstance(ClassInstance&& other) :
    classInfo(other.classInfo),
    instance(other.instance) {

    other.instance = nullptr;
    other.classInfo = nullptr;
}

ClassInstance& ClassInstance::operator=(ClassInstance&& other) {
    if(this == &other) {
        return *this;
    }
    if(instance) {
        auto deleteFunc = classInfo->getDeleteFunction();
        deleteFunc(instance);
    }
    instance = other.instance;
    classInfo = other.classInfo;
    other.instance = nullptr;
    other.classInfo = nullptr;
    return *this;
}

ClassInstance::ClassInstance(const ClassInfo& clsInfo, void* clsInstance) :
    classInfo(&clsInfo),
    instance(clsInstance) {
}

ClassInstance::~ClassInstance() {
    if(instance) {
        auto deleteFunc = classInfo->getDeleteFunction();
        deleteFunc(instance);
    }
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