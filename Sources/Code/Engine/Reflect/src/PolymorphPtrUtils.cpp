#include "PolymorphPtrUtils.hpp"
#include "Core/JSONNode.hpp"
#include "Reflect/ClassInfo.hpp"
#include "Reflect/ETReflectInterfaces.hpp"

namespace {

bool IsClassDerivededFrom(TypeId baseClassId, const ClassInfo& classInfo) {
    ClassInfo* baseClassInfo = nullptr;
    ET_SendEventReturn(baseClassInfo, &ETClassInfoManager::ET_findClassInfoByTypeId, baseClassId);
    if(!baseClassInfo) {
        LogError("[IsClassDerivededFrom] Can't find base class info for type id '%d'", baseClassId);
        return false;
    }
    if(!classInfo.isDerivedFrom(*baseClassInfo)) {
        LogError("[IsClassDerivededFrom] Class '%s' isn't derived from required base class '%s'",
            classInfo.getName(), baseClassInfo->getName());
        return false;
    }
    return true;
}

} // namespace

namespace ReflectUtils {

bool ReadPolyPtrFrom(const SerializeContext& ctx, ClassInstance& instance, TypeId baseClassId, const JSONNode& node) {
    if(!node.hasKey("type")) {
        LogWarning("[ReadPolyPtrFrom] Can't find required field 'type'");
        return false;
    }
    if(!node.hasKey("data")) {
        LogWarning("[ReadPolyPtrFrom] Can't find required field 'data'");
        return false;
    }
    std::string typeName;
    node.read("type", typeName);
    if(typeName.empty()) {
        LogWarning("[ReadPolyPtrFrom] Polymorph object type is empty");
        return false;
    }
    auto currClassInfo = instance.getClassInfo();
    if(!currClassInfo || typeName != currClassInfo->getName()) {
        ClassInfo* newClassInfo = nullptr;
        ET_SendEventReturn(newClassInfo, &ETClassInfoManager::ET_findClassInfoByName, typeName.c_str());
        if(!newClassInfo) {
            LogWarning("[ReadPolyPtrFrom] Can't find class info for a type: '%s'", typeName);
            return false;
        }
        if(!IsClassDerivededFrom(baseClassId, *newClassInfo)) {
            return false;
        }
        currClassInfo = newClassInfo;
        instance = newClassInfo->createInstance();
        if(!instance.get()) {
            LogError("[ReadPolyPtrFrom] Can't create instance of class: '%s'", typeName);
            return false;
        }
    }
    if(!instance.readAllValuesFrom(ctx, node.object("data"))) {
        LogError("[ReadPolyPtrFrom] Can't read instance values of class: '%s'", typeName);
        return false;
    }
    return true;
}

bool WritePolyPtrTo(const SerializeContext& ctx, ClassInstance& instance, JSONNode& node) {
    auto classInfo = instance.getClassInfo();
    if(!classInfo) {
        node.write("type", "null");
        node.write("data", JSONNode());
        return true;
    }
    node.write("type", classInfo->getName());
    JSONNode dataNode;
    if(!instance.writeAllValuesTo(ctx, dataNode)) {
        LogError("[WritePolyPtrTo] Can't write instance values of class: '%s'", classInfo->getName());
        return false;
    }
    node.write("data", dataNode);
    return true;
}

} // namespace ReflectUtils