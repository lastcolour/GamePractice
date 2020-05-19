#ifndef __ENTITY_HPP__
#define __ENTITY_HPP__

#include "Entity/ETEntityInterfaces.hpp"
#include "Reflect/ClassInstance.hpp"

#include <string>
#include <memory>

class EntityLogic;

class Entity : public ETNode<ETEntity> {

public:

    Entity(const char* entityName, EntityId entId);
    virtual ~Entity();

    EntityLogicId addLogic(ClassInstance&& logicInstance);
    EntityLogicId addCustomLogic(std::unique_ptr<EntityLogic>&& logicPtr);
    bool removeLogic(EntityLogicId logicId);
    bool dumpLogicData(EntityLogicId logicId, MemoryStream& stream);

    EntityId getEntityId() const { return entityId; }

    // ETEntity
    const char* ET_getName() const override;
    void ET_setParent(EntityId entId) override;
    void ET_addChild(EntityId entId) override;
    void ET_removeChild(EntityId entId) override;
    EntityId ET_getParentId() const override;
    const Transform& ET_getTransform() const override;
    void ET_setTransform(const Transform& transform) override;
    const std::vector<EntityId>& ET_getChildren() const override;
    int ET_getMaxChildrenDepth() const override;

private:

    Entity& operator=(const Entity&) = delete;
    Entity(const Entity&) = delete;

private:

    EntityLogicId createLogicId() const;
    ClassInstance* findLogic(EntityLogicId logicId);

private:

    struct EntityLogicNode {
        ClassInstance logic;
        int logicId;
    };

private:

    Transform tm;
    std::vector<EntityLogicNode> logics;
    std::vector<EntityId> children;
    std::string name;
    EntityId parentId;
    EntityId entityId;
};

#endif /* __ENTITY_HPP__ */