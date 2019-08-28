#include "Entity/Entity.hpp"
#include "Entity/EntityLogic.hpp"

#include <cassert>

Entity::Entity(const char* entityName, EntityId entId) :
    name(entityName),
    entityId(entId) {

    assert(!name.empty() && "Invalid entity name");

    ETNode<ETEntity>::connect(getEntityId());
}

Entity::~Entity() {
    for(auto it = children.rbegin(), end = children.rend(); it != end; ++it) {
        ET_SendEvent(&ETEntityManager::ET_destroyEntity, *it);
    }
}

void Entity::addLogic(Entity::EntityLogicPtrT&& logic) {
    assert(logic && "Add invalid game logic");
    logic->setEntity(this);
    logics.emplace_back(std::move(logic));
}

const char* Entity::ET_getName() const {
    return name.c_str();
}

const std::vector<EntityId>& Entity::ET_getChildren() const {
    return children;
}

void Entity::ET_setParent(EntityId entId) {
    assert(entId != entityId && "Can't set self as a parent");
    if(parentId == entId) {
        return;
    }
    if(parentId != InvalidEntityId) {
        ET_SendEvent(parentId, &ETEntity::ET_removeChild, entityId);
    }
    parentId = entId;
    if(parentId != InvalidEntityId) {
        ET_SendEvent(parentId, &ETEntity::ET_addChild, entityId);
    }
}

void Entity::ET_addChild(EntityId entId) {
    assert(entId != entityId && "Can't add self as a child");
    assert(entId != InvalidEntityId && "Invalid entity id");
    for(auto childId : children) {
        if(childId == entId) {
            return;
        }
    }
    children.push_back(entId);
    EntityId entParentId;
    ET_SendEventReturn(entParentId, entId, &ETEntity::ET_getParentId);
    if(entParentId != entityId) {
        ET_SendEvent(entId, &ETEntity::ET_setParent, entityId);
    }
    ET_SendEvent(entityId, &ETEntityEvents::ET_onChildAdded, entId);
}

void Entity::ET_removeChild(EntityId entId) {
    auto it = children.begin();
    for(auto end = children.end(); it != end; ++it) {
        if(*it == entId) {
            break;
        }
    }
    if(it != children.end()) {
        EntityId entParentId;
        ET_SendEventReturn(entParentId, entId, &ETEntity::ET_getParentId);
        if(entParentId == entityId) {
            ET_SendEvent(entId, &ETEntity::ET_setParent, InvalidEntityId);
        }
        children.erase(it);
    }
}

EntityId Entity::ET_getParentId() const {
    return parentId;
}

const Transform& Entity::ET_getTransform() const {
    return tm;
}

void Entity::ET_setTransform(const Transform& transform) {
    Vec3 ptOffset = transform.pt - tm.pt;
    tm = transform;
    for(auto childId : children) {
        Transform childTm;
        ET_SendEventReturn(childTm, childId, &ETEntity::ET_getTransform);
        childTm.pt += ptOffset;
        ET_SendEvent(childId, &ETEntity::ET_setTransform, childTm);
    }
    ET_SendEvent(entityId, &ETEntityEvents::ET_onTransformChanged, tm);
}