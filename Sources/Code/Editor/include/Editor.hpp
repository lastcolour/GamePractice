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

LIB_EXPORT uint32_t GetRegisteredEntityLogics(void** out);

LIB_EXPORT void DeInitialize();

LIB_EXPORT uint32_t LoadEntity(const char* entityName);

LIB_EXPORT void UnloadEntity(uint32_t entityId);

LIB_EXPORT uint32_t GetEntityChildEntityId(uint32_t entityId, int32_t childId);

LIB_EXPORT int32_t CreateChildEntity(uint32_t entityId, const char* childName);

LIB_EXPORT const char* GetEntityName(uint32_t entityId);

LIB_EXPORT int32_t AddLogicToEntity(uint32_t entityId, const char* logicName);

LIB_EXPORT void RemoveLogicFromEntity(uint32_t entityId, int32_t logicId);

LIB_EXPORT uint32_t GetEntityLogicData(uint32_t entityId, int32_t logicId, int32_t valueId, void** out);

LIB_EXPORT void SetEntityLogicData(uint32_t entityId, int32_t logicId, int32_t valueId, const void* data, uint32_t size);

LIB_EXPORT void AddEntityLogicArrayElement(uint32_t entityId, int32_t logicId, int32_t valueId);

LIB_EXPORT int32_t AddChildEntityToEntity(uint32_t parentId, uint32_t childId);

LIB_EXPORT void RemoveChildEntityFromEntity(uint32_t parentEntityId, uint32_t childEntityId);

LIB_EXPORT void DrawFrame(void* out, uint32_t w, uint32_t h);

LIB_EXPORT void MouseInputEvent(uint32_t eventType, uint32_t xPos, uint32_t yPos);

LIB_EXPORT void UnloadAll();

} // extern "C"

#endif /* __EDITOR_HPP__ */