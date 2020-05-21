#include "Reflect/ClassInfo.hpp"
#include "Reflect/ETReflectInterfaces.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Core/JSONNode.hpp"
#include "Core/ETPrimitives.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/MemoryStream.hpp"

#include <cassert>

namespace {

const int MAX_PRIMITIVE_PROPERTIES = 255;

} // namespace

ClassInfo::ClassInfo(const char* name, TypeId typeId) :
    className(name),
    instanceTypeId(typeId),
    primitiveValueCount(0) {
}

ClassInfo::~ClassInfo() {
}

ClassInfo::DeleteFuncT ClassInfo::getDeleteFunction() const {
    return deleteFunc;
}

TypeId ClassInfo::getIntanceTypeId() const {
    return instanceTypeId;
}

bool ClassInfo::serializeInstance(void* instance, const JSONNode& node) {
    if(!instance) {
        LogError("[ClassInfo::serializeInstance] Can't serialize instance of class '%s' (Error: null instance)",
            className);
        assert(false && "null instance");
        return false;
    }
    if(!node) {
        LogError("[ClassInfo::serializeInstance] Can't serialize instance of class '%s' (Error: invalid json node)",
            className);
        return false;
    }
    for(auto baseClass : baseClasses) {
        if(!baseClass->serializeInstance(instance, node)) {
            LogError("[ClassInfo::serializeInstance] Can't serialize instance of class '%s' (Error: can't serialize base class '%s')",
                className, baseClass->className);
            return false;
        }
    }
    for(auto& val : values) {
        auto ptr = getValueFunc(instance, val.ptr);
        if(!val.readValue(instance, ptr, node)) {
            LogError("[ClassInfo::serializeInstance] Can't serialize instance of class '%s' (Error: can't serialize field: '%s')",
                className, val.name);
            return false;
        }
    }
    return true;
}

const char* ClassInfo::getName() const {
    return className.c_str();
}

void ClassInfo::registerClassValue(const char* valueName, ClassValueType valueType, ClassValue::ValuePtrT valuePtr, TypeId valueTypeId,
    ClassValue::SetResourceFuncT valueSetFunc) {
    const char* errStr = "[ClassInfo::registerClassValue] Can't register field '%s' for class '%s' (Error: %s)";
    if(!valueName || !valueName[0]) {
        LogError(errStr, valueName, className, "empty name");
        assert(false && "can't register class value");
        return;
    }
    if(valueType == ClassValueType::Invalid) {
        LogError(errStr, valueName, className, "unknown field type");
        assert(false && "can't register class value");
        return;
    }
    if(valueType == ClassValueType::Resource) {
        if(!valueSetFunc) {
            LogError(errStr, valueName, className, "resource without create function");
            assert(false && "can't register class value");
            return;
        }
    } else if (valueType == ClassValueType::Enum) {
        EnumInfo* enumInfo = nullptr;
        ET_SendEventReturn(enumInfo, &ETClassInfoManager::ET_findEnumInfoByTypeId, valueTypeId);
        if(!enumInfo) {
            LogError(errStr, valueName, className, "can't find enum info");
            assert(false && "can't register class value");
            return;
        }
    } else {
        if(!valuePtr) {
            LogError(errStr, valueName, className, "null pointer to value");
            assert(false && "can't register class value");
            return;
        }
        if(valueTypeId == InvalidTypeId) {
            LogError(errStr, valueName, className, "invalid type of field");
            assert(false && "can't register class value");
            return;
        }
    }
    std::vector<ClassInfo*> allBaseClasses;
    getAllClasses(allBaseClasses);
    for(auto baseClass : allBaseClasses) {
        if(baseClass->findValueByName(valueName)) {
            LogError(errStr, valueName, className,
                StringFormat("base class '%s' already registered field with the same name", baseClass->getName()));
            assert(false && "can't register class value");
            return;
        }
        if(valueType != ClassValueType::Resource) {
            if(auto baseValue = baseClass->findValueByPtr(valuePtr)) {
                LogError(errStr, valueName, className,
                    StringFormat("base class '%s' already registered field with other name '%s'",
                    baseClass->getName(), baseValue->name.c_str()));
                assert(false && "can't register class value");
                return;
            }
        }
    }

    ClassValue classValue;
    classValue.name = valueName;
    classValue.type = valueType;
    classValue.ptr = valuePtr;
    classValue.typeId = valueTypeId;
    classValue.setResourceFunc = valueSetFunc;
    classValue.primitiveValueCount = 1;

    if(valueType == ClassValueType::Object) {
        ClassInfo* valueClassInfo = nullptr;
        ET_SendEventReturn(valueClassInfo, &ETClassInfoManager::ET_findClassInfoByTypeId, valueTypeId);
        if(!valueClassInfo) {
            assert(false && "Can't find value class info");
            return;
        }
        classValue.primitiveValueCount = valueClassInfo->primitiveValueCount;
    }

    primitiveValueCount += classValue.primitiveValueCount;
    if(primitiveValueCount >= MAX_PRIMITIVE_PROPERTIES) {
        LogError(errStr, valueName, className, "reach properties limit");
        assert(false && "Too many properties");
        return;
    }

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

ClassInstance ClassInfo::createDefaultInstance() {
    if(!createFunc) {
        return ClassInstance();
    }
    auto object = createFunc();
    if(!object) {
        return ClassInstance();
    }
    return ClassInstance(*this, object);
}

void ClassInfo::getAllClasses(std::vector<ClassInfo*>& classes) {
    for(auto classInfo : baseClasses) {
        classInfo->getAllClasses(classes);
    }
    classes.push_back(this);
}

void ClassInfo::registerBaseClass(TypeId baseClassTypeId) {
    const char* errStr = "[ClassInfo::registerBaseClass] Can't register the base class '%s' for the class '%s' (Error: %s)";

    if(baseClassTypeId == InvalidTypeId) {
        LogError(errStr, "null", className, "invalid base class type id");
        assert(false && "can't register base class");
        return;
    }

    if(!values.empty()) {
        LogError(errStr, "null", className, "Add base class after value was added");
        assert(false && "Base should be registered first");
        return;
    }

    ClassInfo* baseClassInfo = nullptr;
    ET_SendEventReturn(baseClassInfo, &ETClassInfoManager::ET_findClassInfoByTypeId, baseClassTypeId);
    if(!baseClassInfo) {
        LogError(errStr, "null", className, "can't find class info by type id");
        return;
    }

    int newPrimitiveValueCount = 0;
    std::vector<ClassInfo*> newClasses;
    baseClassInfo->getAllClasses(newClasses);

    int currentPrimitiveValueCount = 0;
    std::vector<ClassInfo*> currentClasses;
    getAllClasses(currentClasses);

    for(auto newClass : newClasses) {
        newPrimitiveValueCount += newClass->primitiveValueCount;
        for(auto currentClass : currentClasses) {
            if(newClass == currentClass) {
                LogError(errStr, baseClassInfo->className, className, "class already registered as a base");
                assert(false && "can't register base class");
                return;
            }
            if(newClass->className == currentClass->className) {
                LogError(errStr, baseClassInfo->className, className, "class with the same name already registered");
                assert(false && "can't register base class");
                return;
            }
            if(newClass->instanceTypeId == currentClass->instanceTypeId) {
                LogError(errStr, baseClassInfo->className, className, "change name of the class");
                assert(false && "can't register base class");
                return;
            }
            for(auto& val : newClass->values) {
                if(currentClass->findValueByName(val.name.c_str())) {
                    LogError(errStr, baseClassInfo->className, className, StringFormat("field '%s' already registered", val.name));
                    assert(false && "can't register base class");
                    return;
                }
            }
        }
    }

    primitiveValueCount += newPrimitiveValueCount;
    if(primitiveValueCount >= MAX_PRIMITIVE_PROPERTIES) {
        LogError(errStr, baseClassInfo->className, className, "reach limit of primitive values");
        assert(false && "Too many primitive values");
        return;
    }

    baseClasses.push_back(baseClassInfo);
}

const ClassValue* ClassInfo::findValueByPtr(ClassValue::ValuePtrT ptr) const {
    for(auto& val : values) {
        if(val.ptr == ptr) {
            return &val;
        }
    }
    return nullptr;
}

const ClassValue* ClassInfo::findValueByName(const char* name) const {
    if(!name || !name[0]) {
        return nullptr;
    }
    for(auto& val : values) {
        if(val.name == name) {
            return &val;
        }
    }
    return nullptr;
}

ClassValue* ClassInfo::findValueById(int valueId) {
    if(valueId >= primitiveValueCount) {
        return nullptr;
    }
    std::vector<ClassInfo*> allClasses;
    getAllClasses(allClasses);

    for(auto classInfo : allClasses) {
        if(valueId >= classInfo->primitiveValueCount) {
            valueId -= classInfo->primitiveValueCount;
            continue;
        }
        if(classInfo->primitiveValueCount == static_cast<int>(classInfo->values.size())) {
            return &classInfo->values[valueId];
        }
        for(auto& value : classInfo->values) {
            if(valueId >= value.primitiveValueCount) {
                valueId -= value.primitiveValueCount;
                continue;
            }
            if(value.type == ClassValueType::Object) {
                ClassInfo* valueClassInfo = nullptr;
                ET_SendEventReturn(valueClassInfo, &ETClassInfoManager::ET_findClassInfoByTypeId, value.typeId);
                if(!valueClassInfo) {
                    return nullptr;
                }
                return valueClassInfo->findValueById(valueId);
            }
            return &value;
        }
    }
    return nullptr;
}

void ClassInfo::makeReflectModel(JSONNode& node) {
    node.write("type", "class");

    JSONNode baseNode;
    for(auto baseClass : baseClasses) {
        baseNode.write(baseClass->getName());
    }
    node.write("base", baseNode);

    JSONNode fieldsNode;
    for(auto& value : values) {
        fieldsNode.write(value.name.c_str(), value.getTypeName());
    }
    node.write("data", fieldsNode);
}

bool ClassInfo::readValues(void* instance, MemoryStream& stream) {
    assert(instance && "Invalid instance");

    if(!stream.isOpenedForWrite()) {
        LogError("[ClassInfo::readValues] Can't output class values of class '%s' to non-write stream",
            className);
        return false;
    }

    std::vector<ClassInfo*> allClasses;
    getAllClasses(allClasses);

    for(auto classInfo : allClasses) {
        for(auto& value : classInfo->values) {
            auto ptr = getValueFunc(instance, value.ptr);
            if(!value.readValue(instance, ptr, stream)) {
                LogError("[ClassInfo::readValues] Can't read value of '%s' from class '%s'",
                    value.name, className);
                return false;
            }
        }
    }

    return true;
}

bool ClassInfo::readValue(void* instance, EntityLogicValueId valueId, MemoryStream& stream) {
    assert(instance && "Invalid instance");
    if(!stream.isOpenedForWrite()) {
        LogError("[ClassInfo::readValue] Can't output class value with id '%d' of class '%s' to non-write stream",
            valueId, className);
        return false;
    }
    if(valueId == InvalidEntityLogicValueId) {
        LogError("ClassInfo::readValue] Can't read value with invalid id");
        return false;
    }
    int primitiveValueId = static_cast<int>(valueId);
    primitiveValueId--;
    auto value = findValueById(primitiveValueId);
    if(!value) {
        LogError("ClassInfo::readValue] Can't find value with id '%d' in class '%s'", valueId, className);
        return false;
    }
    auto ptr = getValueFunc(instance, value->ptr);
    if(!value->readValue(instance, ptr, stream)) {
        LogError("[ClassInfo::readValue] Can't read value of '%s' from class '%s'",
            value->name, className);
        return false;
    }
    return true;
}

bool ClassInfo::writeValues(void* instance, MemoryStream& stream) {
    assert(instance && "Invalid instance");

    if(!stream.isOpenedForRead()) {
        LogError("[ClassInfo::writeValues] Can't set class values of class '%s' from non-read stream",
            className);
        return false;
    }

    std::vector<ClassInfo*> allClasses;
    getAllClasses(allClasses);

    for(auto classInfo : allClasses) {
        for(auto& value : classInfo->values) {
            auto ptr = getValueFunc(instance, value.ptr);
            if(!value.writeValue(instance, ptr, stream)) {
                LogError("[ClassInfo::writeValues] Can't write value of '%s' from class '%s'",
                    value.name, className);
                return false;
            }
        }
    }

    return true;
}

bool ClassInfo::writeValue(void* instance, EntityLogicValueId valueId, MemoryStream& stream) {
    assert(instance && "Invalid instance");
    if(!stream.isOpenedForRead()) {
        LogError("[ClassInfo::writeValue] Can't set class value with id '%d' of class '%s' from non-read stream",
            valueId, className);
        return false;
    }
    if(valueId == InvalidEntityLogicValueId) {
        LogError("ClassInfo::writeValue] Can't write value with invalid id");
        return false;
    }
    int primitiveValueId = static_cast<int>(valueId);
    primitiveValueId--;
    auto value = findValueById(primitiveValueId);
    if(!value) {
        LogError("ClassInfo::writeValue] Can't find value with id '%d' in class '%s'", valueId, className);
        return false;
    }
    auto ptr = getValueFunc(instance, value->ptr);
    if(!value->writeValue(instance, ptr, stream)) {
        LogError("[ClassInfo::writeValue] Can't write value of '%s' from class '%s'",
            value->name, className);
        return false;
    }
    return true;
}