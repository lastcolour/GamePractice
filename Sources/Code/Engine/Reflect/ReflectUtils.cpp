#include "Reflect/ReflectUtils.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Reflect/ETReflectInterfaces.hpp"
#include "Reflect/ArrayInfo.hpp"
#include "ETApplicationInterfaces.hpp"

#include <memory>
#include <cassert>

namespace ReflectUtils {

bool ReflectClassByCall(TypeId instanceTypeId, ReflectFuncT reflectFunc) {
    ClassInfo* classInfo = nullptr;
    ET_SendEventReturn(classInfo, &ETClassInfoManager::ET_findClassInfoByTypeId, instanceTypeId);
    if(classInfo) {
        return true;
    }
    ReflectContext ctx;
    if(!ctx.reflectEmbedded(reflectFunc)) {
        LogError("[ReflectUtils::ReflectClassByCall] Can't rreflect embedded class info");
        assert(false && "false reflect result");
        return false;
    }
    return true;
}

bool RegisterArrayInfo(TypeId elemTypeId, ClassValueType elemType, ArrayCreateElemFuncT createFunc, ArraySizeFuncT sizeFunc, ArrayEraseElemFuncT eraseFunc) {
    ArrayInfo* arrayInfo = nullptr;
    ET_SendEventReturn(arrayInfo, &ETClassInfoManager::ET_findArrayInfoByElemTypeId, InvalidTypeId);
    if(arrayInfo) {
        return true;
    }
    std::unique_ptr<ArrayInfo> arrayInfoPtr(new ArrayInfo(elemTypeId, elemType, createFunc, sizeFunc, eraseFunc));
    bool res = false;
    ET_SendEventReturn(res, &ETClassInfoManager::ET_registerArrayInfo, arrayInfoPtr);
    if(!res) {
        LogError("[ReflectUtils::RegisterArrayInfo] Can't register array info");
        assert(false && "false register result");
        return false;
    }
    return true;
}

} // namespace ReflectUtils