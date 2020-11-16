#include "Reflect/ReflectUtils.hpp"
#include "Reflect/ETReflectInterfaces.hpp"
#include "Core/ETAssets.hpp"
#include "Core/ETPrimitives.hpp"
#include "Core/ETLogger.hpp"

#include <cassert>

namespace ReflectUtils {

ClassInfo* FindClassInfo(TypeId typeId) {
    ClassInfo* classInfo = nullptr;
    ET_SendEventReturn(classInfo, &ETClassInfoManager::ET_findClassInfoByTypeId, typeId);
    return classInfo;
}

bool WriteInstanceToLocalFile(void* object, ClassInfo* classInfo, const char* fileName) {
    assert(object && "Invalid object");
    if(!classInfo) {
        LogError("[WriteInstanceToLocalFile] Can't find class info for an object");
        return false;
    }
    JSONNode node;
    SerializeContext serCtx;
    if(!classInfo->writeValueTo(serCtx, object, AllEntityLogicValueId, node)) {
        return false;
    }
    bool saveRes = false;
    auto buffer = node.flushToBuffer();
    if(!buffer) {
        LogError("[WriteInstanceToLocalFile] Can't dump JSON to a buffer");
        return false;
    }
    ET_SendEventReturn(saveRes, &ETAssets::ET_saveLocalFile, fileName, buffer);
    return saveRes;
}

bool ReadIntanceFromLocalFile(void* object, ClassInfo* classInfo, const char* fileName) {
    assert(object && "Invalid object");
    if(!classInfo) {
        LogError("[WriteInstanceToLocalFile] Can't find class info for an object");
        return false;
    }
    JSONNode node;
    ET_SendEventReturn(node, &ETAssets::ET_loadLocalJSONFile, fileName);
    if(!node) {
        return false;
    }
    SerializeContext serCtx;
    if(!classInfo->readValueFrom(serCtx, object, AllEntityLogicValueId, node)) {
        return false;
    }
    return true;
}

bool ReadInstanceFromAsset(void* object, ClassInfo* classInfo, const char* assetName) {
    assert(object && "Invalid object");
    if(!classInfo) {
        LogError("[ReadInstanceFromAsset] Can't find class info for an object");
        return false;
    }
    JSONNode node;
    ET_SendEventReturn(node, &ETAssets::ET_loadJSONAsset, assetName);
    if(!node) {
        return false;
    }
    SerializeContext serCtx;
    if(!classInfo->readValueFrom(serCtx, object, AllEntityLogicValueId, node)) {
        return false;
    }
    return true;
}

} // namespace ReflectUtils