#include "Reflect/ClassValue.hpp"
#include "Reflect/ETReflectInterfaces.hpp"
#include "Reflect/ClassInfo.hpp"
#include "Reflect/EnumInfo.hpp"
#include "Reflect/ArrayInfo.hpp"
#include "Core/ETPrimitives.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"
#include "Core/MemoryStream.hpp"

#include <cassert>

namespace {

template<typename T>
T& getRef(void* value) {
    return *(reinterpret_cast<T*>(value));
}

template<typename T>
const T& getRef(const void* value) {
    return *(reinterpret_cast<const T*>(value));
}

template<typename T>
void readJSONValue(bool isElement, const std::string& name, T& value, const JSONNode& node) {
    if(isElement) {
        node.read(value);
    } else {
        node.read(name.c_str(), value);
    }
} 

bool readJSONVec2i(void* valuePtr, const JSONNode& node) {
    if(!node.hasKey("x")) {
        LogError("[ClassValue::readValue] Can't find required node 'x' to read 'vec2i'");
        return false;
    }
    if(!node.hasKey("y")) {
        LogError("[ClassValue::readValue] Can't find required node 'y' to read 'vec2i'");
        return false;
    }
    int xVal = 0;
    int yVal = 0;
    node.read("x", xVal);
    node.read("y", yVal);
    getRef<Vec2i>(valuePtr) = Vec2i(xVal, yVal);
    return true;
}

bool readJSONVec2(void* valuePtr, const JSONNode& node) {
    if(!node.hasKey("x")) {
        LogError("[ClassValue::readValue] Can't find required node 'x' to read 'vec2'");
        return false;
    }
    if(!node.hasKey("y")) {
        LogError("[ClassValue::readValue] Can't find required node 'y' to read 'vec2'");
        return false;
    }
    float xVal = 0.f;
    float yVal = 0.f;
    node.read("x", xVal);
    node.read("y", yVal);
    getRef<Vec2>(valuePtr) = Vec2(xVal, yVal);
    return true;
}

bool readJSONVec3(void* valuePtr, const JSONNode& node) {
    if(!node.hasKey("x")) {
        LogError("[ClassValue::readValue] Can't find required node 'x' to read 'vec3'");
        return false;
    }
    if(!node.hasKey("y")) {
        LogError("[ClassValue::readValue] Can't find required node 'y' to read 'vec3'");
        return false;
    }
    if(!node.hasKey("z")) {
        LogError("[ClassValue::readValue] Can't find required node 'z' to read 'vec3'");
        return false;
    }
    float xVal = 0.f;
    float yVal = 0.f;
    float zVal = 0.f;
    node.read("x", xVal);
    node.read("y", yVal);
    node.read("z", zVal);
    getRef<Vec3>(valuePtr) = Vec3(xVal, yVal, zVal);
    return true;
}

bool readJSONVec4(void* valuePtr, const JSONNode& node) {
    if(!node.hasKey("x")) {
        LogError("[ClassValue::readValue] Can't find required node 'x' to read 'vec4'");
        return false;
    }
    if(!node.hasKey("y")) {
        LogError("[ClassValue::readValue] Can't find required node 'y' to read 'vec4'");
        return false;
    }
    if(!node.hasKey("z")) {
        LogError("[ClassValue::readValue] Can't find required node 'z' to read 'vec4'");
        return false;
    }
    if(!node.hasKey("w")) {
        LogError("[ClassValue::readValue] Can't find required node 'w' to read 'vec4'");
        return false;
    }
    float xVal = 0.f;
    float yVal = 0.f;
    float zVal = 0.f;
    float wVal = 0.f;
    node.read("x", xVal);
    node.read("y", yVal);
    node.read("z", zVal);
    node.read("w", wVal);
    getRef<Vec4>(valuePtr) = Vec4(xVal, yVal, zVal, wVal);
    return true;
}

bool readJSONQuat(void* valuePtr, const JSONNode& node) {
    if(!node.hasKey("x")) {
        LogError("[ClassValue::readValue] Can't find required node 'x' to read 'quat'");
        return false;
    }
    if(!node.hasKey("y")) {
        LogError("[ClassValue::readValue] Can't find required node 'y' to read 'quat'");
        return false;
    }
    if(!node.hasKey("z")) {
        LogError("[ClassValue::readValue] Can't find required node 'z' to read 'quat'");
        return false;
    }
    if(!node.hasKey("w")) {
        LogError("[ClassValue::readValue] Can't find required node 'w' to read 'quat'");
        return false;
    }
    float xVal = 0.f;
    float yVal = 0.f;
    float zVal = 0.f;
    float wVal = 0.f;
    node.read("x", xVal);
    node.read("y", yVal);
    node.read("z", zVal);
    node.read("w", wVal);
    getRef<Quat>(valuePtr) = Quat(xVal, yVal, zVal, wVal);
    return true;
}

bool readJSONColor(void* valuePtr, const JSONNode& node) {
    if(!node.hasKey("r")) {
        LogError("[ClassValue::readValue] Can't find required node 'r' to read 'color'");
        return false;
    }
    if(!node.hasKey("g")) {
        LogError("[ClassValue::readValue] Can't find required node 'g' to read 'color'");
        return false;
    }
    if(!node.hasKey("b")) {
        LogError("[ClassValue::readValue] Can't find required node 'b' to read 'color'");
        return false;
    }
    if(!node.hasKey("a")) {
        LogError("[ClassValue::readValue] Can't find required node 'a' to read 'color'");
        return false;
    }
    int rVal = 0;
    int gVal = 0;
    int bVal = 0;
    int aVal = 0;
    node.read("r", rVal);
    node.read("g", gVal);
    node.read("b", bVal);
    node.read("a", aVal);
    if(rVal < 0 || rVal > 255) {
        LogError("[ClassValue::readValue] Read value 'r' of 'color' type is out of limits - [0, 255]");
        return false;
    }
    if(gVal < 0 || gVal > 255) {
        LogError("[ClassValue::readValue] Read value 'g' of 'color' type is out of limits - [0, 255]");
        return false;
    }
    if(bVal < 0 || bVal > 255) {
        LogError("[ClassValue::readValue] Read value 'b' of 'color' type is out of limits - [0, 255]");
        return false;
    }
    if(aVal < 0 || aVal > 255) {
        LogError("[ClassValue::readValue] Read value 'a' of 'color' type is out of limits - [0, 255]");
        return false;
    }
    getRef<ColorB>(valuePtr) = ColorB(
        static_cast<uint8_t>(rVal),
        static_cast<uint8_t>(gVal),
        static_cast<uint8_t>(bVal),
        static_cast<uint8_t>(aVal)
    );
    return true;
}

} // namespace

ClassValue::ClassValue() :
    name(),
    type(ClassValueType::Invalid),
    ptr(),
    typeId(InvalidTypeId),
    primitiveValueCount(0),
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
        case ClassValueType::Vec2i: {
            return "vec2i";
        }
        case ClassValueType::Vec2: {
            return "vec2";
        }
        case ClassValueType::Vec3: {
            return "vec3";
        }
        case ClassValueType::Vec4: {
            return "vec4";
        }
        case ClassValueType::Quat: {
            return "quat";
        }
        case ClassValueType::Color: {
            return "color";
        }
        case ClassValueType::Resource: {
            return "resource";
        }
        case ClassValueType::Enum: {
            EnumInfo* enumInfo = nullptr;
            ET_SendEventReturn(enumInfo, &ETClassInfoManager::ET_findEnumInfoByTypeId, typeId);
            if(enumInfo) {
                return enumInfo->getName();
            }
            assert(false && "Can't find enumInfo for object type value");
            return nullptr;
        }
        case ClassValueType::Array: {
            ArrayInfo* arrayInfo = nullptr;
            ET_SendEventReturn(arrayInfo, &ETClassInfoManager::ET_findArrayInfoByElemTypeId, typeId);
            if(arrayInfo) {
                return arrayInfo->getName();
            }
            assert(false && "Can't find arrayInfo for object type value");
            return nullptr;
        }
        case ClassValueType::Entity: {
            return "entity";
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

bool ClassValue::readValue(void* instance, void* valuePtr, const JSONNode& node) {
    if(!isElement) {
        if(!node.hasKey(name.c_str())) {
            LogError("[ClassValue::readValue] Can't find required field '%s'", name);
            return false;
        }
    }
    switch(type)
    {
    case ClassValueType::Invalid: {
        LogError("[ClassValue::readValue] Field '%s' has invalid type", name);
        return false;
    }
    case ClassValueType::Bool: {
        bool value = false;
        readJSONValue(isElement, name, value, node);
        getRef<bool>(valuePtr) = value;
        return true;
    }
    case ClassValueType::Int: {
        int value = 0;
        readJSONValue(isElement, name, value, node);
        getRef<int>(valuePtr) = value;
        return true;
    }
    case ClassValueType::Float: {
        float value = 0.f;
        readJSONValue(isElement, name, value, node);
        getRef<float>(valuePtr) = value;
        return true;
    }
    case ClassValueType::String: {
        std::string value;
        readJSONValue(isElement, name, value, node);
        getRef<std::string>(valuePtr) = value;
        return true;
    }
    case ClassValueType::Object: {
        ClassInfo* classInfo = nullptr;
        ET_SendEventReturn(classInfo, &ETClassInfoManager::ET_findClassInfoByTypeId, typeId);
        if(!classInfo) {
            LogError("[ClassValue::readValue] Can't find class info for a field '%s'", name);
            return false;
        }
        if(!isElement) {
            auto objectNode = node.object(name.c_str());
            if(!objectNode) {
                LogError("[ClassValue::readValue] Can't get object from data for a field '%s'", name);
                return false;
            }
            if(!classInfo->serializeInstance(valuePtr, objectNode)) {
                LogError("[ClassValue::readValue] Can't serialize object field '%s'", name);
                return false;
            }
        } else {
            if(!classInfo->serializeInstance(valuePtr, node)) {
                LogError("[ClassValue::readValue] Can't serialize object of array");
                return false;
            }
        }
        return true;
    }
    case ClassValueType::Vec2i: {
        bool res = false;
        if(isElement) {
            res = readJSONVec2i(valuePtr, node);
        } else {
            res = readJSONVec2i(valuePtr, node.object(name.c_str()));
        }
        if(!res) {
            LogError("[ClassValue::readValue] Can't serialize 'vec2i' field: %s", name);
            return false;
        }
        return true;
    }
    case ClassValueType::Vec2: {
        bool res = false;
        if(isElement) {
            res = readJSONVec2(valuePtr, node);
        } else {
            res = readJSONVec2(valuePtr, node.object(name.c_str()));
        }
        if(!res) {
            LogError("[ClassValue::readValue] Can't serialize 'vec2' field: %s", name);
            return false;
        }
        return true;
    }
    case ClassValueType::Vec3: {
        bool res = false;
        if(isElement) {
            res = readJSONVec3(valuePtr, node);
        } else {
            res = readJSONVec3(valuePtr, node.object(name.c_str()));
        }
        if(!res) {
            LogError("[ClassValue::readValue] Can't serialize 'vec3' field: %s", name);
            return false;
        }
        return true;
    }
    case ClassValueType::Vec4: {
        bool res = false;
        if(isElement) {
            res = readJSONVec4(valuePtr, node);
        } else {
            res = readJSONVec4(valuePtr, node.object(name.c_str()));
        }
        if(!res) {
            LogError("[ClassValue::readValue] Can't serialize 'vec4' field: %s", name);
            return false;
        }
        return true;
    }
    case ClassValueType::Quat: {
        bool res = false;
        if(isElement) {
            res = readJSONQuat(valuePtr, node);
        } else {
            res = readJSONQuat(valuePtr, node.object(name.c_str()));
        }
        if(!res) {
            LogError("[ClassValue::readValue] Can't serialize 'quat' field: %s", name);
            return false;
        }
        return true;
    }
    case ClassValueType::Color: {
        bool res = false;
        if(isElement) {
            res = readJSONColor(valuePtr, node);
        } else {
            res = readJSONColor(valuePtr, node.object(name.c_str()));
        }
        if(!res) {
            LogError("[ClassValue::readValue] Can't serialize 'color' field: %s", name);
            return false;
        }
        return true;
    }
    case ClassValueType::Array: {
        if(isElement) {
            LogError("[ClassValue::readValue] Array of arrays is not supported: '%s'", name);
            return false;
        }
        ArrayInfo* arrayInfo = nullptr;
        ET_SendEventReturn(arrayInfo, &ETClassInfoManager::ET_findArrayInfoByElemTypeId, typeId);
        if(!arrayInfo) {
            LogError("[ClassValue::readValue] Can't find array info for a field '%s'", name);
            return false;
        }
        auto arrayNode = node.object(name.c_str());
        if(!arrayNode) {
            LogError("[ClassValue::readValue] Can't get array object from data for a field '%s'", name);
            return false;
        }
        if(!arrayInfo->readValues(valuePtr, arrayNode)) {
            LogError("[ClassValue::readValue] Can't read array values from data for a field '%s'", name);
            return false;
        }
        return true;
    }
    case ClassValueType::Enum: {
        EnumInfo* enumInfo = nullptr;
        ET_SendEventReturn(enumInfo, &ETClassInfoManager::ET_findEnumInfoByTypeId, typeId);
        if(!enumInfo) {
            LogError("[ClassValue::readValue] Can't find enum info for a field '%s'", name);
            return false;
        }
        std::string value;
        readJSONValue(isElement, name, value, node);
        if(!enumInfo->readValue(valuePtr, value)) {
            LogError("[ClassValue::readValue] Can't serialize enum value for a field '%s'", name);
            return false;
        }
        return true;
    }
    case ClassValueType::Resource: {
        std::string value;
        readJSONValue(isElement, name, value, node);
        setResourceFunc(instance, value.c_str());
        return true;
    }
    default:
        LogError("[ClassValue::readValue] Unknown value type '%s'", name);
        assert(false && "Unknown value type");
        return false;
    }
}

bool ClassValue::readValue(void* instance, void* valuePtr, MemoryStream& stream) const {
    switch(type) {
    case ClassValueType::Bool: {
        stream.write(getRef<bool>(valuePtr));
        break;
    }
    case ClassValueType::Int: {
        stream.write(getRef<int>(valuePtr));
        break;
    }
    case ClassValueType::Float: {
        stream.write(getRef<float>(valuePtr));
        break;
    }
    case ClassValueType::String: {
        stream.write(getRef<std::string>(valuePtr).c_str());
        break;
    }
    case ClassValueType::Vec2i: {
        const auto& val = getRef<Vec2i>(valuePtr);
        stream.write(val.x);
        stream.write(val.y);
        break;
    }
    case ClassValueType::Vec2: {
        const auto& val = getRef<Vec2>(valuePtr);
        stream.write(val.x);
        stream.write(val.y);
        break;
    }
    case ClassValueType::Vec3: {
        const auto& val = getRef<Vec3>(valuePtr);
        stream.write(val.x);
        stream.write(val.y);
        stream.write(val.z);
        break;
    }
    case ClassValueType::Vec4: {
        const auto& val = getRef<Vec4>(valuePtr);
        stream.write(val.x);
        stream.write(val.y);
        stream.write(val.z);
        stream.write(val.w);
        break;
    }
    case ClassValueType::Quat: {
        const auto& val = getRef<Quat>(valuePtr);
        stream.write(val.x);
        stream.write(val.y);
        stream.write(val.z);
        stream.write(val.w);
        break;
    }
    case ClassValueType::Color: {
        const auto& val = getRef<ColorB>(valuePtr);
        stream.write(val.r);
        stream.write(val.g);
        stream.write(val.b);
        stream.write(val.a);
        break;
    }
    case ClassValueType::Object: {
        ClassInfo* classInfo = nullptr;
        ET_SendEventReturn(classInfo, &ETClassInfoManager::ET_findClassInfoByTypeId, typeId);
        if(!classInfo) {
            LogError("[ClassValue::readValue] Can't find class info for a field '%s'", name);
            return false;
        }
        return classInfo->readValues(valuePtr, stream);
    }
    case ClassValueType::Resource: {
        stream.write("");
        break;
    }
    case ClassValueType::Enum: {
        stream.write(getRef<int>(valuePtr));
        break;
    }
    case ClassValueType::Array: {
        ArrayInfo* arrayInfo = nullptr;
        ET_SendEventReturn(arrayInfo, &ETClassInfoManager::ET_findArrayInfoByElemTypeId, typeId);
        if(!arrayInfo) {
            LogError("[ClassValue::readValue] Can't find array info for a field '%s'", name);
            return false;
        }
        return arrayInfo->readValues(valuePtr, stream);
    }
    case ClassValueType::Entity: {
        break;
    }
    case ClassValueType::Invalid:
    default:
        assert(false && "Invalid value type");
        return false;
    }
    return true;
}

bool ClassValue::writeValue(void* instance, void* valuePtr, MemoryStream& stream) {
    switch(type) {
    case ClassValueType::Bool: {
        bool val = false;
        stream.read(val);
        getRef<bool>(valuePtr) = val;
        break;
    }
    case ClassValueType::Int: {
        int val = 0;
        stream.read(val);
        getRef<int>(valuePtr) = val;
        break;
    }
    case ClassValueType::Float: {
        float val = 0.f;
        stream.read(val);
        getRef<float>(valuePtr) = val;
        break;
    }
    case ClassValueType::String: {
        std::string val;
        stream.read(val);
        getRef<std::string>(valuePtr) = val;
        break;
    }
    case ClassValueType::Vec2i: {
        int xVal = 0;
        int yVal = 0;
        stream.read(xVal);
        stream.read(yVal);
        getRef<Vec2i>(valuePtr) = Vec2i(xVal, yVal);
        break;
    }
    case ClassValueType::Vec2: {
        float xVal = 0.f;
        float yVal = 0.f;
        stream.read(xVal);
        stream.read(yVal);
        getRef<Vec2>(valuePtr) = Vec2(xVal, yVal);
        break;
    }
    case ClassValueType::Vec3: {
        float xVal = 0.f;
        float yVal = 0.f;
        float zVal = 0.f;
        stream.read(xVal);
        stream.read(yVal);
        stream.read(zVal);
        getRef<Vec3>(valuePtr) = Vec3(xVal, yVal, zVal);
        break;
    }
    case ClassValueType::Vec4: {
        float xVal = 0.f;
        float yVal = 0.f;
        float zVal = 0.f;
        float wVal = 0.f;
        stream.read(xVal);
        stream.read(yVal);
        stream.read(zVal);
        stream.read(wVal);
        getRef<Vec4>(valuePtr) = Vec4(xVal, yVal, zVal, wVal);
        break;
    }
    case ClassValueType::Quat: {
        float xVal = 0.f;
        float yVal = 0.f;
        float zVal = 0.f;
        float wVal = 0.f;
        stream.read(xVal);
        stream.read(yVal);
        stream.read(zVal);
        stream.read(wVal);
        getRef<Quat>(valuePtr) = Quat(xVal, yVal, zVal, wVal);
        break;
    }
    case ClassValueType::Color: {
        uint8_t rVal = 0u;
        uint8_t gVal = 0u;
        uint8_t bVal = 0u;
        uint8_t aVal = 0u;
        stream.read(rVal);
        stream.read(gVal);
        stream.read(bVal);
        stream.read(aVal);
        getRef<ColorB>(valuePtr) = ColorB(rVal, gVal, bVal, aVal);
        break;
    }
    case ClassValueType::Object: {
        ClassInfo* classInfo = nullptr;
        ET_SendEventReturn(classInfo, &ETClassInfoManager::ET_findClassInfoByTypeId, typeId);
        if(!classInfo) {
            LogError("[ClassValue::readValue] Can't find class info for a field '%s'", name);
            return false;
        }
        return classInfo->writeValues(valuePtr, stream);
    }
    case ClassValueType::Resource: {
        std::string val;
        stream.read(val);
        setResourceFunc(instance, val.c_str());
        break;
    }
    case ClassValueType::Enum: {
        int val = 0;
        stream.read(val);
        getRef<int>(valuePtr) = val;
        break;
    }
    case ClassValueType::Array: {
        ArrayInfo* arrayInfo = nullptr;
        ET_SendEventReturn(arrayInfo, &ETClassInfoManager::ET_findArrayInfoByElemTypeId, typeId);
        if(!arrayInfo) {
            LogError("[ClassValue::readValue] Can't find array info for a field '%s'", name);
            return false;
        }
        return arrayInfo->writeValues(valuePtr, stream);
    }
    case ClassValueType::Entity: {
        break;
    }
    case ClassValueType::Invalid:
    default:
        assert(false && "Invalid value type");
        return false;
    }
    return true;
}