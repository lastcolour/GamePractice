#include "Entity/Entity.hpp"
#include "Entity/EntityLogic.hpp"
#include "ETApplicationInterfaces.hpp"

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
    for(auto it = logics.rbegin(), end = logics.rend(); it != end; ++it) {
        static_cast<EntityLogic*>(it->logic.get())->deinit();
    }
}

EntityLogicId Entity::createLogicId() const {
    if(logics.empty()) {
        return 0;
    }
    return logics.back().logicId + 1;
}

EntityLogicId Entity::addLogic(ClassInstance&& logicInstance) {
    assert(logicInstance.get() && "Can't add null game logic");
    auto logicPtr = static_cast<EntityLogic*>(logicInstance.get());
    logicPtr->setEntity(this);
    if(!logicPtr->init()) {
        return InvalidEntityLogicId;
    }
    auto logicId = createLogicId();
    logics.emplace_back(EntityLogicNode{std::move(logicInstance), logicId});
    return logicId;
}

EntityLogicId Entity::addCustomLogic(std::unique_ptr<EntityLogic>&& logic) {
    assert(logic && "Can't add null custom game logic");
    ClassInstance logicInstance = ClassInstance::CreateWithoutClassInfo(logic.release());
    return addLogic(std::move(logicInstance));
}

bool Entity::removeLogic(EntityLogicId logicId) {
    assert(logicId != InvalidEntityLogicId && "Invalid logic id");
    auto it = logics.begin();
    while(it != logics.end()) {
        if(it->logicId == logicId) {
            break;
        }
        ++it;
    }
    if(it == logics.end()) {
        return false;
    }
    logics.erase(it);
    return true;
}

ClassInstance* Entity::findLogic(EntityLogicId logicId) {
    for(auto& logicNode : logics) {
        if(logicNode.logicId == logicId) {
            return &logicNode.logic;
        }
    }
    return nullptr;
}

bool Entity::dumpLogicData(EntityLogicId logicId, MemoryStream& stream) {
    assert(logicId != InvalidEntityLogicId && "Invalid logic id");
    auto logicInstance = findLogic(logicId);
    if(!logicInstance) {
        LogWarning("[Entity::dumpLogicData] Can't find logic with id %d in entity '%s'", logicId, ET_getName());
        return false;
    }
    return logicInstance->dumpValues(stream);
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
    Vec3 scaleFactor = transform.scale / tm.scale;
    tm = transform;
    for(auto childId : children) {
        Transform childTm;
        ET_SendEventReturn(childTm, childId, &ETEntity::ET_getTransform);
        childTm.pt += ptOffset;
        childTm.scale = Vec3(childTm.scale.x * scaleFactor.x,
            childTm.scale.y * scaleFactor.y,
            childTm.scale.z * scaleFactor.z);
        ET_SendEvent(childId, &ETEntity::ET_setTransform, childTm);
    }
    ET_SendEvent(entityId, &ETEntityEvents::ET_onTransformChanged, tm);
}

int Entity::ET_getMaxChildrenDepth() const {
    int maxDepth = 0;
    for(auto childId : children) {
        int childMaxDepth = 0;
        ET_SendEventReturn(childMaxDepth, childId, &ETEntity::ET_getMaxChildrenDepth);
        childMaxDepth++;
        if(childMaxDepth > maxDepth) {
            maxDepth = childMaxDepth;
        }
    }
    return maxDepth;
}