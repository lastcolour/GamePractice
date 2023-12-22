#include <cassert>

namespace Reflect {

ClassInstance::ClassInstance() :
    instance(nullptr),
    classInfo(nullptr),
    deleteFunc(nullptr) {
}

ClassInstance::ClassInstance(ClassInstance&& other) :
    instance(other.instance),
    classInfo(other.classInfo),
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
        if(classInfo) {
            classInfo->removeInstance(instance);
        } else {
            deleteFunc(instance, EDelOpType::Delete);
        }
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
    instance(clsInstance),
    classInfo(&clsInfo) {
}

ClassInstance::~ClassInstance() {
    if(instance) {
        if(classInfo) {
            classInfo->removeInstance(instance);
        } else {
            deleteFunc(instance, EDelOpType::Delete);
        }
    }
    instance = nullptr;
}

bool ClassInstance::isInstanceOfType(Core::TypeId typeId) const {
    if(!classInfo) {
        return false;
    }
    return classInfo->getIntanceTypeId() == typeId;
}

ClassInfo* ClassInstance::getClassInfo() {
    return classInfo;
}

Core::TypeId ClassInstance::getInstanceTypeId() const {
    if(!classInfo) {
        return Core::InvalidTypeId;
    }
    return classInfo->getIntanceTypeId();
}

void* ClassInstance::get() {
    return instance;
}

const void* ClassInstance::get() const {
    return instance;
}

void ClassInstance::setDeleteFuncAndPtr(DeleteFuncT deleteF, void* ptr) {
    assert(!classInfo && "Intance has class info");
    instance = ptr;
    deleteFunc = deleteF;
}

bool ClassInstance::readAllValuesFrom(const SerializeContext& ctx, const JSONNode& node) {
    return readValueFrom(ctx, AllClassValuesId, node);
}

bool ClassInstance::readAllValuesFrom(const SerializeContext& ctx, Memory::MemoryStream& stream) {
    return readValueFrom(ctx, AllClassValuesId, stream);
}

bool ClassInstance::writeAllValuesTo(const SerializeContext& ctx, JSONNode& node) {
    return writeValueTo(ctx, AllClassValuesId, node);
}

bool ClassInstance::writeAllValuesTo(const SerializeContext& ctx, Memory::MemoryStream& stream) {
    return writeValueTo(ctx, AllClassValuesId, stream);
}

bool ClassInstance::readValueFrom(const SerializeContext& ctx, ClassValueId valueId, const JSONNode& node) {
    if(!instance) {
        assert(false && "Invalid instance");
        return false;
    }
    if(!classInfo) {
        LogError("[ClassInstance::readValueFrom] Can't read value of an instance without a class info");
        return false;
    }
    return classInfo->readValueFrom(ctx, instance, valueId, node);
}

bool ClassInstance::readValueFrom(const SerializeContext& ctx, ClassValueId valueId, Memory::MemoryStream& stream) {
    if(!instance) {
        assert(false && "Invalid instance");
        return false;
    }
    if(!classInfo) {
        LogError("[ClassInstance::readValueFrom] Can't read value of an instance without a class info");
        return false;
    }
    return classInfo->readValueFrom(ctx, instance, valueId, stream);
}

bool ClassInstance::writeValueTo(const SerializeContext& ctx, ClassValueId valueId, JSONNode& node) {
    if(!instance) {
        assert(false && "Invalid instance");
        return false;
    }
    if(!classInfo) {
        LogError("[ClassInstance::writeValueTo] Can't write value of an instance without a class info");
        return false;
    }
    return classInfo->writeValueTo(ctx, instance, valueId, node);
}

bool ClassInstance::writeValueTo(const SerializeContext& ctx, ClassValueId valueId, Memory::MemoryStream& stream) {
    if(!instance) {
        assert(false && "Invalid instance");
        return false;
    }
    if(!classInfo) {
        LogError("[ClassInstance::writeValueTo] Can't write value of an instance without a class info");
        return false;
    }
    return classInfo->writeValueTo(ctx, instance, valueId, stream);
}

bool ClassInstance::addValueArrayElement(ClassValueId valueId) {
    if(!instance) {
        assert(false && "Invalid instance");
        return false;
    }
    if(!classInfo) {
        LogError("[ClassInstance::addValueArrayElement] Can't add array element value to an instance without a class info");
        return false;
    }
    return classInfo->addNewValueArrayElement(instance, valueId);
}

bool ClassInstance::setValuePolymorphType(ClassValueId valueId, const char* typeName) {
    if(!instance) {
        assert(false && "Invalid instance");
        return false;
    }
    if(!classInfo) {
        LogError("[ClassInstance::setValuePolymorphType] Can't set value polymorph type to an instance without a class info");
        return false;
    }
    return classInfo->setValuePolymorphType(instance, valueId, typeName);
}

void ClassInstance::reCreate() {
    assert(instance && "No instance to re-create!");

    if(classInfo) {
        if(!classInfo->reCreateInstance(instance)) {
            assert(false && "Can't re-create instance");
        }
    } else {
        if(!deleteFunc(instance, EDelOpType::ReCreate)) {
            assert(false && "Can't re-create instance");
        }
    }
}

} // namespace Reflect