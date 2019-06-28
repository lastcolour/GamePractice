#include "Game/GameObject.hpp"
#include "Game/GameLogic.hpp"

#include <cassert>

GameObject::GameObject(const std::string& objectName, EntityId entId) :
    name(objectName),
    entityId(entId) {

    assert(!name.empty() && "Invalid object name");

    ETNode<ETGameObject>::connect(getEntityId());
}

GameObject::~GameObject() {
    for(auto it = children.rbegin(), end = children.rend(); it != end; ++it) {
        ET_SendEvent(&ETGameObjectManager::ET_destroyObject, *it);
    }
}

void GameObject::addLogic(std::unique_ptr<GameLogic>&& logic) {
    assert(logic && "Add invalid game logic");
    logic->setGameObject(this);
    logics.emplace_back(std::move(logic));
}

const std::string& GameObject::ET_getName() const {
    return name;
}

void GameObject::ET_setParent(EntityId entId) {
    assert(entId != entityId && "Can't set self as a parent");
    if(parentId == entId) {
        return;
    }
    if(parentId != InvalidEntityId) {
        ET_SendEvent(parentId, &ETGameObject::ET_removeChild, entityId);
    }
    parentId = entId;
    if(parentId != InvalidEntityId) {
        ET_SendEvent(parentId, &ETGameObject::ET_addChild, entityId);
    }
}

void GameObject::ET_addChild(EntityId entId) {
    assert(entId != entityId && "Can't add self as a child");
    assert(entId != InvalidEntityId && "Invalid entity id as a object");
    for(auto childId : children) {
        if(childId == entId) {
            return;
        }
    }
    children.push_back(entId);
    EntityId entParentId;
    ET_SendEventReturn(entParentId, entId, &ETGameObject::ET_getParentId);
    if(entParentId != entityId) {
        ET_SendEvent(entId, &ETGameObject::ET_setParent, entityId);
    }
}

void GameObject::ET_removeChild(EntityId entId) {
    auto it = children.begin();
    for(auto end = children.end(); it != end; ++it) {
        if(*it == entId) {
            break;
        }
    }
    if(it != children.end()) {
        EntityId entParentId;
        ET_SendEventReturn(entParentId, entId, &ETGameObject::ET_getParentId);
        if(entParentId == entityId) {
            ET_SendEvent(entId, &ETGameObject::ET_setParent, InvalidEntityId);
        }
        children.erase(it);
    }
}

EntityId GameObject::ET_getParentId() const {
    return parentId;
}

const Transform& GameObject::ET_getTransform() const {
    return tm;
}

void GameObject::ET_setTransform(const Transform& transform) {
    tm = transform;
}