#ifndef __ENTITY_HPP__
#define __ENTITY_HPP__

#include "Entity/ETEntityInterfaces.hpp"
#include "Reflect/ClassInstance.hpp"

#include <string>
#include <memory>

class EntityLogic;

class EntityLogicNode {
public:

    EntityLogicNode(ClassInstance&& instance, EntityLogicId id) :
        logic(std::move(instance)), logicId(id) {}
    EntityLogicNode(EntityLogicNode&& other) :
        logic(std::move(other.logic)), logicId(other.logicId) {
    }
    EntityLogicNode& operator=(EntityLogicNode&& other) {
        if(this != &other) {
            logic = std::move(other.logic);
            logicId = other.logicId;
        }
        return *this;
    }

private:

    EntityLogicNode(EntityLogicNode& other) = delete;
    EntityLogicNode& operator=(EntityLogicNode& other) = delete;

public:

    ClassInstance logic;
    EntityLogicId logicId;
};

class Entity : public ETNode<ETEntity> {
public:

    struct EntityChildNode {
        EntityId childEntId;
        EntityChildId childId;
    };

public:

    Entity(const char* entityName, EntityId entId);
    virtual ~Entity();

    void addChildEntityWithId(EntityChildId childId, Entity& entity);
    bool addLogicWithId(EntityLogicId logicId, ClassInstance&& logicInstance);
    EntityLogicId addLogic(ClassInstance&& logicInstance);
    EntityLogicId addCustomLogic(std::unique_ptr<EntityLogic>&& logicPtr);
    bool removeLogic(EntityLogicId logicId);
    bool readLogicData(EntityLogicId logicId, EntityLogicValueId valueId, MemoryStream& stream);
    bool writeLogicData(EntityLogicId logicId, EntityLogicValueId valueId, MemoryStream& stream);
    bool addLogicValueArrayElemet(EntityLogicId logicId, EntityLogicValueId valueId);
    Transform* getTransform();

    EntityId getEntityId() const { return entityId; }

    // ETEntity
    const char* ET_getName() const override;
    void ET_setParent(EntityId entId) override;
    EntityChildId ET_addChild(EntityId entId) override;
    void ET_removeChild(EntityId entId) override;
    EntityId ET_getParentId() const override;
    const Transform& ET_getTransform() const override;
    void ET_setTransform(const Transform& transform) override;
    int ET_getMaxChildrenDepth() const override;
    EntityChildId ET_getChildIdFromEntityId(EntityId childEntId) const override;
    EntityId ET_getEntityIdFromChildId(EntityChildId childId) const override;

private:

    Entity& operator=(const Entity&) = delete;
    Entity(const Entity&) = delete;

private:

    ClassInstance* findLogic(EntityLogicId logicId);
    EntityLogicId createNewLogicId() const;
    EntityChildId createNewChildId() const;

private:

    Transform tm;
    std::vector<EntityLogicNode> logics;
    std::vector<EntityChildNode> children;
    std::string name;
    EntityId parentId;
    EntityId entityId;
};

#endif /* __ENTITY_HPP__ */