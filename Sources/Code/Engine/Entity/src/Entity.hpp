#ifndef __ENTITY_HPP__
#define __ENTITY_HPP__

#include "Entity/ETEntity.hpp"
#include "Core/ETPrimitives.hpp"
#include "Reflect/ClassInstance.hpp"

#include <string>
#include <memory>

class EntityLogic;
class EntityRegistry;

class Entity : public ETNode<ETEntity> {
public:

    struct EntityChildNode {
        Entity* childEntity;
        EntityChildId childId;
    };

    struct EntityLogicNode {
        ClassInstance logic;
        EntityLogicId logicId;
    };

public:

    Entity(const char* entityName, EntityRegistry* entityRegistry, EntityId entId);
    virtual ~Entity();

    void addChildEntityWithId(EntityChildId childId, Entity& entity);
    bool addLogicWithId(EntityLogicId logicId, ClassInstance&& logicInstance);
    EntityLogicId addLogic(ClassInstance&& logicInstance);
    EntityLogicId addCustomLogic(std::unique_ptr<EntityLogic>&& logicPtr);
    bool removeLogic(EntityLogicId logicId);
    bool readLogicData(EntityLogicId logicId, EntityLogicValueId valueId, MemoryStream& stream);
    bool writeLogicData(EntityLogicId logicId, EntityLogicValueId valueId, MemoryStream& stream);
    bool addLogicValueArrayElemet(EntityLogicId logicId, EntityLogicValueId valueId);
    EntityId getEntityId() const { return entityId; }
    void setName(const char* newName);
    void purgeAllRelationships();

    // ETEntity
    const char* ET_getName() const override;
    void ET_setParent(EntityId entId) override;
    EntityChildId ET_addChild(EntityId entId) override;
    void ET_removeChild(EntityId entId) override;
    EntityId ET_getParentId() const override;
    const Transform& ET_getTransform() const override;
    void ET_setTransform(const Transform& transform) override;
    EntityChildId ET_getChildIdFromEntityId(EntityId childEntId) const override;
    EntityId ET_getEntityIdFromChildId(EntityChildId childId) const override;
    Transform ET_getLocalTransform() const override;
    void ET_setLocalTransform(const Transform& localTm) override;
    std::vector<EntityId> ET_getChildren() const override;

private:

    Entity& operator=(const Entity&) = delete;
    Entity(const Entity&) = delete;

private:

    ClassInstance* findLogic(EntityLogicId logicId);
    EntityLogicId createNewLogicId() const;
    EntityChildId createNewChildId() const;

private:

    EntityRegistry* registry;
    Entity* parent;
    Transform tm;
    std::vector<EntityLogicNode> logics;
    std::vector<EntityChildNode> children;
    std::string name;
    EntityId entityId;
};

#endif /* __ENTITY_HPP__ */