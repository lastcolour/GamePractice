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

} // namespace

ClassValue::ClassValue() :
    name(),
    type(ClassValueType::Invalid),
    ptr(),
    typeId(InvalidTypeId) {
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
    if(!node.hasKey(name.c_str())) {
        LogError("[ClassValue::serializeValue] Can't find required field '%s'", name);
        return false;
    }
    switch (type)
    {
    case ClassValueType::Invalid: {
        LogError("[ClassValue::serializeValue] Field '%s' has invalid type", name);
        return false;
    }
    case ClassValueType::Bool: {
        bool value = false;
        node.read(name.c_str(), value);
        getRef<bool>(valuePtr) = value;
        return true;
    }
    case ClassValueType::Int: {
        int value = 0;
        node.read(name.c_str(), value);
        getRef<int>(valuePtr) = value;
        return true;
    }
    case ClassValueType::Float: {
        float value = 0.f;
        node.read(name.c_str(), value);
        getRef<float>(valuePtr) = value;
        return true;
    }
    case ClassValueType::String: {
        std::string value;
        node.read(name.c_str(), value);
        getRef<std::string>(valuePtr) = value;
        return true;
    }
    case ClassValueType::Object: {
        auto objectNode = node.object(name.c_str());
        if(!objectNode) {
            LogError("[ClassValue::serializeValue] Can't get object from data for a field '%s'", name);
            return false;
        }
        ClassInfo* classInfo = nullptr;
        ET_SendEventReturn(classInfo, &ETClassInfoManager::ET_findClassInfoByTypeId, typeId);
        if(!classInfo) {
            LogError("[ClassValue::serializeValue] Can't find class info for a field '%s'", name);
            return false;
        }
        if(!classInfo->serializeInstance(valuePtr, objectNode)) {
            LogError("[ClassValue::serializeValue] Can't serialize object field '%s'", name);
            return false;
        }
        return true;
    }
    case ClassValueType::Array: {
        ArrayInfo* arrayInfo = nullptr;
        for(auto& elemNode : node) {
        }
        return true;
    }
    case ClassValueType::Enum: {
        std::string value;
        node.read(name.c_str(), value);
        EnumInfo* enumInfo = nullptr;
        ET_SendEventReturn(enumInfo, &ETClassInfoManager::ET_findEnumInfoByTypeId, typeId);
        if(!enumInfo) {
            LogError("[ClassValue::serializeValue] Can't find enum info for a field '%s'", name);
            return false;
        }
        if(!enumInfo->serializeValue(valuePtr, value)) {
            LogError("[ClassValue::serializeValue] Can't serialize enum value for a field '%s'", name);
            return false;
        }
        return true;
    }
    case ClassValueType::Resource: {
        std::string value;
        node.read(name.c_str(), value);
        setResourceFunc(instance, value.c_str());
        return true;
    }
    default:
        LogError("[ClassValue::serializeValue] Unknown value type '%s'", name);
        assert(false && "Unknown value type");
        return false;
    }
}