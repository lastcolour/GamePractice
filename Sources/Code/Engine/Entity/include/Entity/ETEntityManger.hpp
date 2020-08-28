#ifndef __ET_ENTITY_MANAGER_HPP__
#define __ET_ENTITY_MANAGER_HPP__

#include "Core/Core.hpp"

class EntityLogicsRegister;
class JSONNode;
class MemoryStream;

struct ETEntityManager {
    virtual ~ETEntityManager() = default;
    virtual EntityId ET_createEntity(const char* entityName) = 0;
    virtual EntityChildId ET_createChildEntity(EntityId parentId, const char* entityName) = 0;
    virtual void ET_destroyEntity(EntityId entId) = 0;
    virtual bool ET_renameEntity(EntityId entId, const char* newName) = 0;
    virtual void ET_destroyAllEntities() = 0;
    virtual bool ET_registerLogics(EntityLogicsRegister& logicsRegister) = 0;
    virtual EntityId ET_createEntityFromJSON(const JSONNode& node, const char* entityName) = 0;
    virtual EntityLogicId ET_addLogicToEntity(EntityId entityId, const char* logicName) = 0;
    virtual void ET_removeLogicFromEntity(EntityId entityId, EntityLogicId logicId) = 0;
    virtual JSONNode ET_getRegisteredLogics() const = 0;
    virtual bool ET_readEntityLogicData(EntityId entityId, EntityLogicId logicId,
        EntityLogicValueId valueId, MemoryStream& stream) = 0;
    virtual bool ET_writeEntityLogicData(EntityId entityId, EntityLogicId logicId,
        EntityLogicValueId valueId, MemoryStream& stream) = 0;
    virtual bool ET_addEntityLogicArrayElement(EntityId entityId, EntityLogicId logicId,
        EntityLogicValueId valueId) = 0;
};

struct ETAsyncEntityManager {
    virtual ~ETAsyncEntityManager() = default;
    virtual void ET_createAsyncEntity(const char* entityName, std::function<void(EntityId)> callback) = 0;
};

#endif /* __ET_ENTITY_MANAGER_HPP__ */