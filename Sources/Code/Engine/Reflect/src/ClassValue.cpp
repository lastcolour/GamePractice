#include "Reflect/ClassValue.hpp"
#include "Reflect/ETReflectInterfaces.hpp"
#include "Reflect/ClassInfo.hpp"
#include "Reflect/EnumInfo.hpp"
#include "ArrayInfo.hpp"
#include "Core/ETLogger.hpp"
#include "Core/JSONNode.hpp"
#include "Core/MemoryStream.hpp"
#include "Entity/ETEntity.hpp"

#include <cassert>

namespace {

EntityId getEntityIdFromChildIdSequence(const SerializeContext& ctx, const std::vector<EntityChildId>& childIdSequence) {
    if(childIdSequence.empty()) {
        return InvalidEntityId;
    }
    if(!ctx.entityId.isValid()) {
        LogError("[ClassValue::getEntityIdFromChildIdSequence] No active entity to query child id");
        assert(false && "Invalid serializable entity");
        return InvalidEntityId;
    }
    EntityId currentParentEntId = ctx.entityId;
    EntityId currentChildEntId;
    for(size_t i = 0, sz = childIdSequence.size(); i < sz; ++i) {
        currentChildEntId = InvalidEntityId;
        auto childId = childIdSequence[i];
        ET_SendEventReturn(currentChildEntId, currentParentEntId, &ETEntity::ET_getEntityIdFromChildId, childId);
        if(!currentChildEntId.isValid()) {
            LogWarning("[ClassValue::getEntityIdFromChildIdSequence] Can't query entity id for a child with id '%d' from an entiy: '%s'",
                childId, EntityUtils::GetEntityName(currentParentEntId));
            break;
        }
        currentParentEntId = currentChildEntId;
    }
    return currentChildEntId;
}

std::vector<EntityChildId> getChildIdSequenceFromEntityId(const SerializeContext& ctx, EntityId childEntId) {
    std::vector<EntityChildId> childIdSequence;
    if(!childEntId.isValid()) {
        return childIdSequence;
    }
    if(!ctx.entityId.isValid()) {
        LogWarning("[ClassValue::getChildIdFromEntityId] No active entity to query child id");
        assert(false && "Invalid serializable entity");
        return childIdSequence;
    }
    if(ctx.entityId == childEntId) {
        EntityChildId childId = InvalidEntityChildId;
        ET_SendEventReturn(childId, ctx.entityId, &ETEntity::ET_getChildIdFromEntityId, childEntId);
        childIdSequence.push_back(childId);
        return childIdSequence;
    }
    EntityId currentChildEntId = childEntId;
    while(true) {
        EntityId parentEntId;
        ET_SendEventReturn(parentEntId, currentChildEntId, &ETEntity::ET_getParentId);
        if(!parentEntId.isValid()) {
            LogWarning("[ClassValue::getChildIdSequenceFromEntityId] Child entity '%s' does not have a parent",
                EntityUtils::GetEntityName(childEntId));
            break;
        }
        EntityChildId childId = InvalidEntityChildId;
        ET_SendEventReturn(childId, parentEntId, &ETEntity::ET_getChildIdFromEntityId, currentChildEntId);
        if(childId == InvalidEntityChildId) {
            LogError("[ClassValue::getChildIdSequenceFromEntityId] Can't get child id for a child entity '%s' from parent: '%s'",
                EntityUtils::GetEntityName(currentChildEntId), EntityUtils::GetEntityName(parentEntId));
            childIdSequence.clear();
            break;
        }
        childIdSequence.push_back(childId);
        if(ctx.entityId == parentEntId) {
            break;
        }
        currentChildEntId = parentEntId;
    }

    return childIdSequence;
}

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

const char* getResourceName(ResourceType resType) {
    switch(resType) {
        case ResourceType::Entity: {
            return "Entity";
        }
        case ResourceType::Sound: {
            return "Sound";
        }
        case ResourceType::SoundEvent: {
            return "SoundEvent";
        }
        case ResourceType::Image: {
            return "Image";
        }
        case ResourceType::Invalid: {
            [[fallthrough]];
        }
        default: {
            assert(false && "Invalid resource type");
        }
    }
    return "";
}

bool readJSONChildIdSequence(std::vector<EntityChildId>& childIdSequence, const JSONNode& node) {
    if(!node.isArray()) {
        LogError("[ClassValue::readJSONChildIdSequence] Entity value type is not an 'array'");
        return false;
    }
    assert(childIdSequence.empty() && "Invalid output arg");
    for(const auto& val : node) {
        EntityChildId childId = InvalidEntityChildId;
        val.read(childId);
        childIdSequence.push_back(childId);
    }
    return true;
}

} // namespace

ClassValue::ClassValue() :
    name(),
    type(ClassValueType::Invalid),
    ptr(),
    typeId(InvalidTypeId),
    resourceType(ResourceType::Invalid),
    primitiveValueCount(0),
    isElement(false) {
}

ClassValue::~ClassValue() {
}

std::string ClassValue::getTypeName() const {
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
            std::string typeName = "resource.";
            typeName += getResourceName(resourceType);
            return typeName;
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

bool ClassValue::readValueFrom(const SerializeContext& ctx, void* instance, void* valuePtr, const JSONNode& node) {
    if(!isElement) {
        if(!node.hasKey(name.c_str())) {
            LogError("[ClassValue::readValueFrom] Can't find required field '%s'", name);
            return false;
        }
    }
    switch(type)
    {
    case ClassValueType::Invalid: {
        LogError("[ClassValue::readValueFrom] Field '%s' has invalid type", name);
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
            LogError("[ClassValue::readValueFrom] Can't find class info for a field '%s'", name);
            return false;
        }
        if(!isElement) {
            auto objectNode = node.object(name.c_str());
            if(!objectNode) {
                LogError("[ClassValue::readValueFrom] Can't get object from data for a field '%s'", name);
                return false;
            }
            if(!classInfo->readValueFrom(ctx, valuePtr, AllEntityLogicValueId, objectNode)) {
                LogError("[ClassValue::readValueFrom] Can't serialize object field '%s'", name);
                return false;
            }
        } else {
            if(!classInfo->readValueFrom(ctx, valuePtr, AllEntityLogicValueId, node)) {
                LogError("[ClassValue::readValueFrom] Can't serialize object of array");
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
            LogError("[ClassValue::readValueFrom] Can't read 'vec2i' field: %s", name);
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
            LogError("[ClassValue::readValueFrom] Can't read 'vec2' field: %s", name);
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
            LogError("[ClassValue::readValueFrom] Can't read 'vec3' field: %s", name);
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
            LogError("[ClassValue::readValueFrom] Can't read 'vec4' field: %s", name);
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
            LogError("[ClassValue::readValueFrom] Can't read 'quat' field: %s", name);
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
            LogError("[ClassValue::readValueFrom] Can't read 'color' field: %s", name);
            return false;
        }
        return true;
    }
    case ClassValueType::Array: {
        if(isElement) {
            LogError("[ClassValue::readValueFrom] Array of arrays is not supported: '%s'", name);
            return false;
        }
        ArrayInfo* arrayInfo = nullptr;
        ET_SendEventReturn(arrayInfo, &ETClassInfoManager::ET_findArrayInfoByElemTypeId, typeId);
        if(!arrayInfo) {
            LogError("[ClassValue::readValueFrom] Can't find array info for a field '%s'", name);
            return false;
        }
        auto arrayNode = node.object(name.c_str());
        if(!arrayNode) {
            LogError("[ClassValue::readValueFrom] Can't get array object from data for a field '%s'", name);
            return false;
        }
        if(!arrayInfo->readValuesFrom(ctx, valuePtr, arrayNode)) {
            LogError("[ClassValue::readValueFrom] Can't read array values from data for a field '%s'", name);
            return false;
        }
        return true;
    }
    case ClassValueType::Enum: {
        EnumInfo* enumInfo = nullptr;
        ET_SendEventReturn(enumInfo, &ETClassInfoManager::ET_findEnumInfoByTypeId, typeId);
        if(!enumInfo) {
            LogError("[ClassValue::readValueFrom] Can't find enum info for a field '%s'", name);
            return false;
        }
        std::string value;
        readJSONValue(isElement, name, value, node);
        if(!enumInfo->readValueFrom(valuePtr, value)) {
            LogError("[ClassValue::readValueFrom] Can't serialize enum value for a field '%s'", name);
            return false;
        }
        return true;
    }
    case ClassValueType::Resource: {
        std::string value;
        readJSONValue(isElement, name, value, node);
        if(setResourceFunc) {
            setResourceFunc(instance, value.c_str());
        } else {
            getRef<std::string>(valuePtr) = value;
        }
        return true;
    }
    case ClassValueType::Entity: {
        bool res = false;
        std::vector<EntityChildId> childIdSequence;
        if(isElement) {
            res = readJSONChildIdSequence(childIdSequence, node);
        } else {
            res = readJSONChildIdSequence(childIdSequence, node.object(name.c_str()));
        }
        if(!res) {
            LogError("[ClassValue::readValueFrom] Can't read 'entityId' field '%s'", name);
            return false;
        }
        getRef<EntityId>(valuePtr) = getEntityIdFromChildIdSequence(ctx, childIdSequence);
        return true;
    }
    default:
        LogError("[ClassValue::readValueFrom] Unknown value type '%s'", name);
        assert(false && "Unknown value type");
        return false;
    }
}

bool ClassValue::writeValueTo(const SerializeContext& ctx, void* instance, void* valuePtr, MemoryStream& stream) {
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
            LogError("[ClassValue::writeValueTo] Can't find class info for a field '%s'", name);
            return false;
        }
        return classInfo->writeValueTo(ctx, valuePtr, AllEntityLogicValueId, stream);
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
            LogError("[ClassValue::writeValueTo] Can't find array info for a field '%s'", name);
            return false;
        }
        return arrayInfo->writeValuesTo(ctx, valuePtr, stream);
    }
    case ClassValueType::Entity: {
        auto entityId = getRef<EntityId>(valuePtr);
        auto childIdSequence = getChildIdSequenceFromEntityId(ctx, entityId);
        stream.write(static_cast<int>(childIdSequence.size()));
        for(auto& val : childIdSequence) {
            stream.write(val);
        }
        break;
    }
    case ClassValueType::Invalid:
    default:
        assert(false && "Invalid value type");
        return false;
    }
    return true;
}

bool ClassValue::readValueFrom(const SerializeContext& ctx, void* instance, void* valuePtr, MemoryStream& stream) {
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
            LogError("[ClassValue::readValueFrom] Can't find class info for a field '%s'", name);
            return false;
        }
        return classInfo->readValueFrom(ctx, valuePtr, AllEntityLogicValueId, stream);
    }
    case ClassValueType::Resource: {
        std::string val;
        stream.read(val);
        if(setResourceFunc) {
            setResourceFunc(instance, val.c_str());
        } else {
            getRef<std::string>(valuePtr) = val;
        }
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
            LogError("[ClassValue::readValueFrom] Can't find array info for a field '%s'", name);
            return false;
        }
        return arrayInfo->readValuesFrom(ctx, valuePtr, stream);
    }
    case ClassValueType::Entity: {
        std::vector<EntityChildId> childIdSequence;
        int idsCount = 0;
        stream.read(idsCount);
        for(int i = 0; i < idsCount; ++i) {
            int val = 0;
            stream.read(val);
            childIdSequence.push_back(val);
        }
        getRef<EntityId>(valuePtr) = getEntityIdFromChildIdSequence(ctx, childIdSequence);
        break;
    }
    case ClassValueType::Invalid:
    default:
        assert(false && "Invalid value type");
        return false;
    }
    return true;
}

bool ClassValue::addArrayElement(void* valuePtr) {
    assert(type == ClassValueType::Array && "Invalid value type");
    ArrayInfo* arrayInfo = nullptr;
    ET_SendEventReturn(arrayInfo, &ETClassInfoManager::ET_findArrayInfoByElemTypeId, typeId);
    if(!arrayInfo) {
        LogError("[ClassValue::addArrayElement] Can't find array info for a field: '%s'", name);
        return false;
    }
    return arrayInfo->addElement(valuePtr);
}

void ClassValue::setDefaultValue(void* valuePtr) {
    switch(type) {
    case ClassValueType::Bool: {
        getRef<bool>(valuePtr) = false;
        break;
    }
    case ClassValueType::Int: {
        getRef<int>(valuePtr) = 0;
        break;
    }
    case ClassValueType::Float: {
        getRef<float>(valuePtr) = 0.f;
        break;
    }
    case ClassValueType::String: {
        break;
    }
    case ClassValueType::Vec2i: {
        getRef<Vec2i>(valuePtr) = Vec2i(0);
        break;
    }
    case ClassValueType::Vec2: {
        getRef<Vec2>(valuePtr) = Vec2(0.f);
        break;
    }
    case ClassValueType::Vec3: {
        getRef<Vec3>(valuePtr) = Vec3(0.f);
        break;
    }
    case ClassValueType::Vec4: {
        getRef<Vec4>(valuePtr) = Vec4(0.f);
        break;
    }
    case ClassValueType::Quat: {
        getRef<Quat>(valuePtr) = Quat(0.f, 0.f, 0.f, 1.f);
        break;
    }
    case ClassValueType::Color: {
        getRef<ColorB>(valuePtr) = ColorB(255, 255, 255, 255);
        break;
    }
    case ClassValueType::Object: {
        break;
    }
    case ClassValueType::Resource: {
        break;
    }
    case ClassValueType::Enum: {
        EnumInfo* enumInfo = nullptr;
        ET_SendEventReturn(enumInfo, &ETClassInfoManager::ET_findEnumInfoByTypeId, typeId);
        if(!enumInfo) {
            LogError("[ClassValue::setDefaultValue] Can't find enum info for a field '%s'", name);
            return;
        }
        getRef<int>(valuePtr) = enumInfo->getDefaultValue();
        break;
    }
    case ClassValueType::Array: {
        assert(false && "Array of arrays not supported");
        break;
    }
    case ClassValueType::Entity: {
        getRef<EntityId>(valuePtr) = InvalidEntityId;
        break;
    }
    case ClassValueType::Invalid:
    default:
        assert(false && "Invalid value type");
    }
}