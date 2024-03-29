#ifndef __REFLECT_UTILS_HPP__
#define __REFLECT_UTILS_HPP__

#include "Reflect/ReflectContext.hpp"

namespace Reflect {

ClassInfo* FindClassInfo(Core::TypeId typeId);

template<typename T>
ClassInfo* CreateClassInfo() {
    ReflectContext ctx;
    ctx.reflect<T>();
    return ctx.getRegisteredClassInfo();
}

template<typename T>
ClassInfo* GetOrCreateClassInfo() {
    auto typeId = Core::GetTypeId<T>();
    auto classInfo = FindClassInfo(typeId);
    if(!classInfo) {
        return CreateClassInfo<T>();
    }
    return classInfo;
}

void AsyncWriteInstaceToLocalFile(void* object, ClassInfo* classInfo, const char* fileName);

bool WriteInstanceToLocalFile(void* object, ClassInfo* classInfo, const char* fileName);

bool ReadIntanceFromLocalFile(void* object, ClassInfo* classInfo, const char* fileName);

bool ReadInstanceFromAsset(void* object, ClassInfo* classInfo, const char* fileName);

template<typename T>
bool LoadObjectFromAsset(T& object, const char* assetName) {
    auto classInfo = GetOrCreateClassInfo<T>();
    return ReadInstanceFromAsset(&object, classInfo, assetName);
}

bool LoadObjectFromAsset(ClassInstance& instance, const char* assetName);

template<typename T>
bool LoadObjectFromLocalFile(T& object, const char* fileName) {
    auto classInfo = GetOrCreateClassInfo<T>();
    return ReadIntanceFromLocalFile(&object, classInfo, fileName);
}

template<typename T>
bool SaveObjectToLocalFile(T& object, const char* fileName) {
    auto classInfo = GetOrCreateClassInfo<T>();
    return WriteInstanceToLocalFile(&object, classInfo, fileName);
}

template<typename T>
void AsyncSaveObjectToLocalFile(T& object, const char* fileName) {
    auto classInfo = GetOrCreateClassInfo<T>();
    return AsyncWriteInstaceToLocalFile(&object, classInfo, fileName);
}

} // namespace Reflect

#endif /* __REFLECT_UTILS_HPP__ */