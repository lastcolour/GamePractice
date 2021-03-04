#ifndef __ENTITY_MANAGER_HPP__
#define __ENTITY_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Entity/ETEntityManger.hpp"
#include "EntityRegistry.hpp"

#include <memory>
#include <string>
#include <unordered_map>

class ClassInfo;

class EntityManager : public SystemLogic,
    public ETNode<ETEntityManager> {
public:

    EntityManager();
    virtual ~EntityManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETEntityManager
    EntityId ET_createEntity(const char* entityName) override;
    EntityId ET_createEntityFromData(const char* entityName, const char* entityData) override;
    Entity* ET_createRawEntity(const char* entityName) override;
    EntityChildId ET_createChildEntity(EntityId parentId, const char* entityName) override;
    void ET_destroyEntity(EntityId targetEntId) override;
    bool ET_renameEntity(EntityId entId, const char* newName) override;
    void ET_destroyAllEntities() override;
    bool ET_registerLogics(EntityLogicsRegister& logicsRegister) override;
    EntityId ET_createEntityFromJSON(const JSONNode& node, const char* entityName) override;
    EntityLogicId ET_addLogicToEntity(EntityId targetEntId, const char* logicName) override;
    void ET_removeLogicFromEntity(EntityId targetEntId, EntityLogicId logicId) override;
    JSONNode ET_getRegisteredLogics() const override;
    bool ET_readEntityLogicData(EntityId targetEntId, EntityLogicId logicId,
        EntityLogicValueId valueId, MemoryStream& stream) override;
    bool ET_writeEntityLogicData(EntityId targetEntId, EntityLogicId logicId,
        EntityLogicValueId valueId, MemoryStream& stream) override;
    bool ET_addEntityLogicArrayElement(EntityId targetEntId, EntityLogicId logicId,
        EntityLogicValueId valueId) override;
    EntityId ET_createUnfinishedEntity(const char* entityName) override;
    bool ET_finishEntity(EntityId targetEntId) override;

private:

    Entity* createEntityImpl(const JSONNode& entityNode, const char* entityName, bool finishLoad);
    Entity* createEntity(const char* entityName, bool finishLoad);
    JSONNode loadEntityRootNode(const char* entityName) const;
    bool setupEntityTranform(Entity* entity, const JSONNode& node);
    bool setupEntityLogics(Entity* entity, const JSONNode& node) const;
    bool setupEntityChildren(Entity* entity, const JSONNode& node, bool finishLoad);

private:

    struct LogicNode {
        const char* moduleName;
        ClassInfo* classInfo;
    };

private:

    EntityRegistry registry;
    std::unordered_map<std::string, LogicNode> registeredLogics;
    ClassInfo* tmClassInfo;
};

#endif /* __ENTITY_MANAGER_HPP__ */