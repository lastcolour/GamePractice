#include "Reflect/ClassIntance.hpp"
#include "Reflect/ClassInfo.hpp"

ClassInstance::ClassInstance() :
    classInfo(nullptr),
    instance(nullptr) {
}

ClassInstance::ClassInstance(const ClassInfo& clsInfo, void* clsInstance) :
    classInfo(&clsInfo),
    instance(clsInstance) {
}

ClassInstance::~ClassInstance() {
    if(!instance) {
        auto deleteFunc = classInfo->getDeleteFunction();
        deleteFunc(instance);
    }
}

bool ClassInstance::isInstanceOfType(TypeId typeId) const {
    return classInfo->getIntanceTypeId() == typeId;
}
