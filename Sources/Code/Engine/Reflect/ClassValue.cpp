#include "Reflect/ClassValue.hpp"
#include "Reflect/ETReflectInterfaces.hpp"
#include "Reflect/ClassInfo.hpp"
#include "Reflect/EnumInfo.hpp"
#include "Reflect/ArrayInfo.hpp"
#include "Core/ETPrimitives.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"

#include <cassert>

namespace {

template<typename T>
T& getRef(void* value) {
    return *(reinterpret_cast<T*>(value));
}

template<typename T>
void readValue(bool isElement, const std::string& name, T& value, const JSONNode& node) {
    if(isElement) {
        node.read(value);
    } else {
        node.read(name.c_str(), value);
    }
}

} // namespace

ClassValue::ClassValue() :
    name(),
    type(ClassValueType::Invalid),
    ptr(),
    typeId(InvalidTypeId),
    isElement(false) {
}

ClassValue::~ClassValue() {
}

const char* ClassValue::getTypeName() const {
    switch(type) {
        case ClassValueType::Bool: {
            return "bool";
        }
        case ClassValueType::Int: {
            return "int";
        }
        case ClassValueType::Float: {
            return "float";
        }
        case ClassValueType::String: {
            return "string";
        }
        case ClassValueType::Object: {
            ClassInfo* clsInfo = nullptr;
            ET_SendEventReturn(clsInfo, &ETClassInfoManager::ET_findClassInfoByTypeId, typeId);
            if(clsInfo) {
                return clsInfo->getName();
            }
            assert(false && "Can't find classInfo for object type value");
            return nullptr;
        }
        case ClassValueType::Invalid: {
            assert(false && "Can't query value type name of invalid value");
            return nullptr;
        }
        default: {
            assert(false && "Unknown value type");
            return nullptr;
        }
    }
}

bool ClassValue::serializeValue(void* instance, void* valuePtr, const JSONNode& node) {
    if(!isElement) {
        if(!node.hasKey(name.c_str())) {
            LogError("[ClassValue::serializeValue] Can't find required field '%s'", name);
            return false;
        }
    }
    switch (type)
    {
    case ClassValueType::Invalid: {
        LogError("[ClassValue::serializeValue] Field '%s' has invalid type", name);
        return false;
    }
    case ClassValueType::Bool: {
        bool value = false;
        readValue(isElement, name, value, node);
        getRef<bool>(valuePtr) = value;
        return true;
    }
    case ClassValueType::Int: {
        int value = 0;
        readValue(isElement, name, value, node);
        getRef<int>(valuePtr) = value;
        return true;
    }
    case ClassValueType::Float: {
        float value = 0.f;
        readValue(isElement, name, value, node);
        getRef<float>(valuePtr) = value;
        return true;
    }
    case ClassValueType::String: {
        std::string value;
        readValue(isElement, name, value, node);
        getRef<std::string>(valuePtr) = value;
        return true;
    }
    case ClassValueType::Object: {
        ClassInfo* classInfo = nullptr;
        ET_SendEventReturn(classInfo, &ETClassInfoManager::ET_findClassInfoByTypeId, typeId);
        if(!classInfo) {
            LogError("[ClassValue::serializeValue] Can't find class info for a field '%s'", name);
            return false;
        }
        if(!isElement) {
            auto objectNode = node.object(name.c_str());
            if(!objectNode) {
                LogError("[ClassValue::serializeValue] Can't get object from data for a field '%s'", name);
                return false;
            }
            if(!classInfo->serializeInstance(valuePtr, objectNode)) {
                LogError("[ClassValue::serializeValue] Can't serialize object field '%s'", name);
                return false;
            }
        } else {
            if(!classInfo->serializeInstance(valuePtr, node)) {
                LogError("[ClassValue::serializeValue] Can't serialize object of array");
                return false;
            }
        }
        return true;
    }
    case ClassValueType::Array: {
        if(isElement) {
            LogError("[ClassValue::serializeValue] Array of arrays is not supported: '%s'", name);
            return false;
        }
        ArrayInfo* arrayInfo = nullptr;
        ET_SendEventReturn(arrayInfo, &ETClassInfoManager::ET_findArrayInfoByElemTypeId, typeId);
        if(!arrayInfo) {
            LogError("[ClassValue::serializeValue] Can't find array info for a field '%s'", name);
            return false;
        }
        auto arrayNode = node.object(name.c_str());
        if(!arrayNode) {
            LogError("[ClassValue::serializeValue] Can't get array object from data for a field '%s'", name);
            return false;
        }
        for(auto& elemNode : arrayNode) {
            auto elem = arrayInfo->createElement(valuePtr);
            if(!elem) {
                LogError("[ClassValue::serializeValue] Can't create new array element for a field '%s'", name);
                return false;
            }
            if(!arrayInfo->serializeElement(elem, elemNode)) {
                LogError("[ClassValue::serializeValue] Can't create array element for a field '%s'", name);
                return false;
            }
        }
        return true;
    }
    case ClassValueType::Enum: {
        EnumInfo* enumInfo = nullptr;
        ET_SendEventReturn(enumInfo, &ETClassInfoManager::ET_findEnumInfoByTypeId, typeId);
        if(!enumInfo) {
            LogError("[ClassValue::serializeValue] Can't find enum info for a field '%s'", name);
            return false;
        }
        std::string value;
        readValue(isElement, name, value, node);
        if(!enumInfo->serializeValue(valuePtr, value)) {
            LogError("[ClassValue::serializeValue] Can't serialize enum value for a field '%s'", name);
            return false;
        }
        return true;
    }
    case ClassValueType::Resource: {
        std::string value;
        readValue(isElement, name, value, node);
        setResourceFunc(instance, value.c_str());
        return true;
    }
    default:
        LogError("[ClassValue::serializeValue] Unknown value type '%s'", name);
        assert(false && "Unknown value type");
        return false;
    }
}