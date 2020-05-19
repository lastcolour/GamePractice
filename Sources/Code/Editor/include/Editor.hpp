#ifndef __EDITOR_HPP__
#define __EDITOR_HPP__

#ifdef _WIN32
  #define LIB_EXPORT __declspec(dllexport)
#else
  #define LIB_EXPORT
#endif

#include <cstdint>

extern "C" {

LIB_EXPORT uint32_t Initiliaze();

LIB_EXPORT const char* GetReflectModel();

LIB_EXPORT void DeInitialize();

LIB_EXPORT uint32_t LoadEntity(const char* entityName);

LIB_EXPORT void UnloadEntity(uint32_t entityId);

LIB_EXPORT uint32_t GetEntityChildren(uint32_t entityId, void** out);

LIB_EXPORT const char* GetEntityName(uint32_t entityId);

LIB_EXPORT int32_t AddLogicToEntity(uint32_t entityId, const char* logicName);

LIB_EXPORT void RemoveLogicFromEntity(uint32_t entityId, int32_t logicId);

LIB_EXPORT uint32_t GetEntityLogicData(uint32_t entityId, int32_t logicId, void** out);

LIB_EXPORT void SetEntityLogicFieldData(uint32_t entityId, int32_t logicId, int32_t fieldId, void* data, uint32_t size);

LIB_EXPORT void DrawFrame(void* out, uint32_t w, uint32_t h);

} // extern "C"

#endif /* __EDITOR_HPP__ */