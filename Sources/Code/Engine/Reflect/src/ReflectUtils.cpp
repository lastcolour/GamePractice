#include "Reflect/ReflectUtils.hpp"
#include "Reflect/ClassInfoManager.hpp"
#include "Core/ETAssets.hpp"

#include <cassert>

namespace Reflect {

ClassInfo* FindClassInfo(Core::TypeId typeId) {
    return GetEnv()->GetClassInfoManager()->findClassInfoByTypeId(typeId);
}

void AsyncWriteInstaceToLocalFile(void* object, ClassInfo* classInfo, const char* fileName) {
    assert(object && "Invalid object");
    if(!classInfo) {
        LogError("[AsyncWriteInstaceToLocalFile] Can't find class info for an object");
        return;
    }
    JSONNode node;
    SerializeContext serCtx;
    if(!classInfo->writeValueTo(serCtx, object, AllClassValuesId, node)) {
        return;
    }
    auto buffer = node.flushToBuffer();
    if(!buffer) {
        LogError("[AsyncWriteInstaceToLocalFile] Can't dump JSON to a buffer");
        return;
    }
    ET_SendEvent(&ETAsyncAssets::ET_asyncSaveLocalFile, fileName, buffer);
}

bool WriteInstanceToLocalFile(void* object, ClassInfo* classInfo, const char* fileName) {
    assert(object && "Invalid object");
    if(!classInfo) {
        LogError("[WriteInstanceToLocalFile] Can't find class info for an object");
        return false;
    }
    JSONNode node;
    SerializeContext serCtx;
    if(!classInfo->writeValueTo(serCtx, object, AllClassValuesId, node)) {
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
    if(!classInfo->readValueFrom(serCtx, object, AllClassValuesId, node)) {
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
    if(!classInfo->readValueFrom(serCtx, object, AllClassValuesId, node)) {
        return false;
    }
    return true;
}

bool LoadObjectFromAsset(ClassInstance& instance, const char* assetName) {
    return ReadInstanceFromAsset(instance.get(), instance.getClassInfo(), assetName);
}

} // namespace Reflect