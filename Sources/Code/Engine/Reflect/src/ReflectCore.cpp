#include "Reflect/ClassInfoManager.hpp"
#include "ArrayInfo.hpp"
#include "ResourceInfo.hpp"

#include <cassert>

namespace Reflect {

namespace Impl {

bool ReflectClassByCall(Core::TypeId instanceTypeId, ReflectFuncT reflectFunc) {
    ClassInfo* classInfo = GetEnv()->GetClassInfoManager()->findClassInfoByTypeId(instanceTypeId);
    if(classInfo) {
        return true;
    }
    ReflectContext ctx;
    if(!ctx.reflectEmbedded(reflectFunc)) {
        LogError("[Reflect::ReflectClassByCall] Can't reflect embedded class info");
        assert(false && "false reflect result");
        return false;
    }
    return true;
}

bool RegisterArrayInfo(Core::TypeId elemTypeId, ClassValueType elemType, ArrayCreateElemFuncT createFunc,
    ArraySizeFuncT sizeFunc, ArrayGetElemFuncT getElemFunc, ArrayResetFuncT resetFunc) {
    ArrayInfo* arrayInfo = GetEnv()->GetClassInfoManager()->findArrayInfoByElemTypeId(elemTypeId);
    if(arrayInfo) {
        return true;
    }
    std::unique_ptr<ArrayInfo> arrayInfoPtr(new ArrayInfo(elemTypeId, elemType, createFunc,
        sizeFunc, getElemFunc, resetFunc));
    if(!GetEnv()->GetClassInfoManager()->registerArrayInfo(arrayInfoPtr)) {
        LogError("[Reflect::RegisterArrayInfo] Can't register array info");
        assert(false && "false register result");
        return false;
    }
    return true;
}

bool RegisterResourceInfo(Core::TypeId resourceTypeId, ClassValueType elemType, Core::TypeId elemTypeId, const char* name,
    ResourceConvertFuncT toRuntimeConvertFunc, ResourceConvertFuncT toStoreConvertFunc, ResourceTempBuffInitFuncT tempBufferInitFunc) {

    ResourceInfo* resourceInfo = GetEnv()->GetClassInfoManager()->findResourceInfoByTypeId(resourceTypeId);
    if(resourceInfo) {
        return true;
    }

    std::unique_ptr<ResourceInfo> resourceInfoPtr(new ResourceInfo(resourceTypeId, elemType, elemTypeId, name,
        toRuntimeConvertFunc, toStoreConvertFunc, tempBufferInitFunc));
    if(!GetEnv()->GetClassInfoManager()->registerResourceInfo(resourceInfoPtr)) {
        LogError("[Reflect::RegisterResourceInfo] Can't register resource info");
        assert(false && "false register result");
        return false;
    }
    return true;
}

} // namespace Impl

} // namespace Reflect