#ifndef __ENTITY_INTERFACES_HPP__
#define __ENTITY_INTERFACES_HPP__

#include "Math/Transform.hpp"
#include "Core/ETPrimitives.hpp"

#include <vector>

class EntityLogicRegister;
class JSONNode;

struct ETEntityEvents {
    virtual ~ETEntityEvents() = default;
    virtual void ET_onTransformChanged(const Transform& newTm) = 0;
    virtual void ET_onChildAdded(EntityId childId) = 0;
};

struct ETEntity {
    virtual ~ETEntity() = default;
    virtual const std::vector<EntityId>& ET_getChildren() const = 0;
    virtual void ET_setParent(EntityId entId) = 0;
    virtual void ET_addChild(EntityId entId) = 0;
    virtual void ET_removeChild(EntityId entId) = 0;
    virtual EntityId ET_getParentId() const = 0;
    virtual const char* ET_getName() const = 0;
    virtual const Transform& ET_getTransform() const = 0;
    virtual void ET_setTransform(const Transform& tm) = 0;
    virtual int ET_getMaxChildrenDepth() const = 0;
};

struct ETEntityManager {
    virtual ~ETEntityManager() = default;
    virtual EntityId ET_createEntity(const char* entityName) = 0;
    virtual void ET_destroyEntity(EntityId entId) = 0;
    virtual bool ET_registerLogics(EntityLogicRegister& logicRegister) = 0;
    virtual EntityId ET_createEntityFromJSON(const JSONNode& node, const char* entityName) = 0;
};

#endif /* __ENTITY_INTERFACES_HPP__ */