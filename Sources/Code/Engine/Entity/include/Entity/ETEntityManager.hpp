#ifndef __ET_ENTITY_MANAGER_HPP__
#define __ET_ENTITY_MANAGER_HPP__

class Entity;
class JSONNode;
class EntityLoadResult;
class EntityLogicsRegister;

struct ETEntityManager {
    virtual ~ETEntityManager() = default;
    virtual EntityId ET_createEntity(const char* entityName) = 0;
    virtual EntityId ET_createUnfinishedEntity(const char* entityName) = 0;
    virtual bool ET_finishEntity(EntityId entityId) = 0;
    virtual EntityId ET_createEntityFromData(const char* entityName, const char* entityData) = 0;
    virtual EntityId ET_createEntityFromJSON(const JSONNode& node, const char* entityName) = 0;
    virtual EntityChildId ET_createChildEntity(EntityId parentId, const char* entityName) = 0;
    virtual Entity* ET_createRawEntity(const char* entityName) = 0;
    virtual void ET_destroyEntity(EntityId destroyEndId) = 0;
    virtual bool ET_renameEntity(EntityId entId, const char* newName) = 0;
    virtual void ET_destroyAllEntities() = 0;
    virtual bool ET_registerLogics(EntityLogicsRegister& logicsRegister) = 0;
    virtual EntityLogicId ET_addLogicToEntity(EntityId targetEntId, const char* logicName) = 0;
    virtual void ET_removeLogicFromEntity(EntityId entityId, EntityLogicId logicId) = 0;
    virtual JSONNode ET_getRegisteredLogics() const = 0;
    virtual bool ET_readEntityLogicData(EntityId entityId, EntityLogicId logicId,
        Reflect::ClassValueId valueId, Memory::MemoryStream& stream) = 0;
    virtual bool ET_writeEntityLogicData(EntityId entityId, EntityLogicId logicId,
        Reflect::ClassValueId valueId, Memory::MemoryStream& stream) = 0;
    virtual bool ET_addEntityLogicArrayElement(EntityId entityId, EntityLogicId logicId,
        Reflect::ClassValueId valueId) = 0;
    virtual bool ET_setEntityLogicPolymorphObjectType(EntityId entityId, EntityLogicId logicId,
        Reflect::ClassValueId valueId, const char* typeName) = 0;
    virtual size_t ET_getEntitiesCount() const = 0;
};

struct ETAsyncEntityManager {
    virtual ~ETAsyncEntityManager() = default;
    virtual void ET_createAsyncEntity(const char* entityName, std::function<void(std::shared_ptr<EntityLoadResult>)> callback) = 0;
    virtual bool ET_isInsideAsyncLoad() const = 0;
    virtual void ET_addEntityToFinishLater(EntityId loadEntId) = 0;
};

#endif /* __ET_ENTITY_MANAGER_HPP__ */