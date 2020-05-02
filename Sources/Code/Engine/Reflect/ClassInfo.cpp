#include "Reflect/ClassInfo.hpp"
#include "Reflect/ETReflectInterfaces.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Core/JSONNode.hpp"
#include "Core/ETPrimitives.hpp"
#include "ETApplicationInterfaces.hpp"

namespace {

template<typename T>
T& getRef(void* value) {
    return *(reinterpret_cast<T*>(value));
}

} // namespace

ClassInfo::ClassInfo() {
}

ClassInfo::~ClassInfo() {
}

bool ClassInfo::initClassInfo(const char* name, TypeId objectTypeId, CreateFuncT createF, DeleteFuncT deleteF, GetValueFuncT getValueF) {
    const char* errStr = "[ClassInfo::initClassInfo] Can't init class info for '%s' (Error: %s)";
    if(!name || !name[0]) {
        LogError(errStr, name, "empty name");
        return false;
    }
    ClassInfo* classInfo = nullptr;
    ET_SendEventReturn(classInfo, &ETClassInfoManager::ET_findClassInfoByName, name);
    if(classInfo) {
        LogError(errStr, name, "already registered by name");
        return false;
    }
    if(objectTypeId == InvalidTypeId) {
        LogError(errStr, name, "instances have invalid type id");
        return false;
    }
    ET_SendEventReturn(classInfo, &ETClassInfoManager::ET_findClassInfoByTypeId, objectTypeId);
    if(classInfo) {
        LogError(errStr, name, "already registered by typeId");
        return false;
    }
    if(!getValueF) {
        LogError(errStr, name, "no get value function");
        return false;
    }
    if(className.empty()) {
        LogError(errStr, name, "dobule initialization");
        return false;
    }
    className = name;
    instanceTypeId = objectTypeId;
    createFunc = createF;
    deleteFunc = deleteF;
    getValueF = getValueF;
    return true;
}

ClassInstanceDeleteFuncT ClassInfo::getDeleteFunction() const {
    return deleteFunc;
}

TypeId ClassInfo::getIntanceTypeId() const {
    return instanceTypeId;
}

bool ClassInfo::serializeInstance(void* instance, const JSONNode& classNode) {
    for(auto& val : values) {
        if(!classNode.hasKey(val.name.c_str())) {
            return false;
        }
        auto ptr = getValueFunc(instance, val.ptr);
        switch (val.type)
        {
        case ClassValueType::Invalid: {
            break;
        }
        case ClassValueType::Bool: {
            bool value = false;
            classNode.read(val.name.c_str(), value);
            getRef<bool>(ptr) = value;
            break;
        }
        case ClassValueType::Int: {
            int value = false;
            classNode.read(val.name.c_str(), value);
            getRef<int>(ptr) = value;
            break;
        }
        case ClassValueType::Float: {
            float value = 0.f;
            classNode.read(val.name.c_str(), value);
            getRef<float>(ptr) = value;
            break;
        }
        case ClassValueType::String: {
            std::string value;
            classNode.read(val.name.c_str(), value);
            getRef<std::string>(ptr) = value;
            break;
        }
        case ClassValueType::Object: {
            auto objectNode = classNode.object(val.name.c_str());
            if(!objectNode) {
                return false;
            }
            ClassInfo* classInfo = nullptr;
            ET_SendEventReturn(classInfo, &ETClassInfoManager::ET_findClassInfoByTypeId, val.typeId);
            if(!classInfo) {
                return false;
            }
            if(!classInfo->serializeInstance(ptr, objectNode)) {
                return false;
            }
            break;
        }
        default:
            return false;
        }
    }
    return true;
}

const char* ClassInfo::getName() const {
    return className.c_str();
}

ClassInfo* ClassInfo::findClassInfo(TypeId instanceTypeId) const {
    ClassInfo* classInfo = nullptr;
    ET_SendEventReturn(classInfo, &ETClassInfoManager::ET_findClassInfoByTypeId, instanceTypeId);
    return classInfo;
}

void ClassInfo::registerClassValue(const char* valueName, ClassValueType valueType, ClassValue::ValuePtrT valuePtr, TypeId valueTypeId,
    ClassValue::CreateFuncT valueCreateFunc) {
    const char* errStr = "[ClassInfo::registerClassValue] Can't register field '%s' for class %s (Error: %s)";
    if(!valueName || !valueName[0]) {
        LogError(errStr, className, valueName, "empty name");
        return;
    }
    if(!valuePtr) {
        LogError(errStr, className, valueName, "null pointer to value");
        return;
    }
    if(valueType == ClassValueType::Invalid) {
        LogError(errStr, className, valueName, "unknown field type");
        return;
    }
    if(valueTypeId == InvalidTypeId) {
        LogError(errStr, className, valueName, "invalid type of field");
        return;
    }
    if(valueType == ClassValueType::Resource) {
        if(!valueCreateFunc) {
            LogError(errStr, className, valueName, "resource without create function");
            return;
        }
    }
    for(const auto& val : values) {
        if(val.name == valueName) {
            LogError(errStr, className, valueName, "double field registration");
            return;
        }
        if(val.ptr == valuePtr) {
            LogError(errStr, className, valueName, "change name of field");
            return;
        }
    }
    ClassValue classValue;
    classValue.name = valueName;
    classValue.type = valueType;
    classValue.ptr = valuePtr;
    classValue.typeId = valueTypeId;
    classValue.createFunc = valueCreateFunc;
    values.push_back(classValue);
}

ClassInstance ClassInfo::createInstance(const JSONNode& node) {
    if(!createFunc) {
        return ClassInstance();
    }
    auto object = createFunc();
    if(!object) {
        return ClassInstance();
    }
    if(!serializeInstance(object, node)) {
        deleteFunc(object);
        return ClassInstance();
    }
    return ClassInstance(*this, object);
}

void ClassInfo::registerBaseClass(ReflectContext& ctx) {
    auto instanceTypeId = ctx.getRegisteredClassInfo()->getIntanceTypeId();
    baseClasses.push_back(instanceTypeId);
}

bool ClassInfo::reflectEmbebedClass(ReflectFuncT reflectFunc) {
    ReflectContext ctx;
    return ctx.reflectEmbedded(reflectFunc);
}

 void ClassInfo::makeReflectModel(JSONNode& node) {
    for(auto& value : values) {
        node.write(value.name.c_str(), value.getTypeName());
    }
}