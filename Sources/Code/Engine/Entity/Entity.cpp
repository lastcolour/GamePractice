#include "Entity/Entity.hpp"
#include "Entity/EntityLogic.hpp"
#include "ETApplicationInterfaces.hpp"

#include <cassert>

namespace {

bool CheckLogicValue(const char* errStr, Entity* entity, EntityLogicId logicId, ClassInstance* logicInstance, EntityLogicValueId valueId) {
    assert(logicId != InvalidEntityLogicId && "Invalid logic id");
    assert(valueId != InvalidEntityLogicValueId && "Invalid logic value id");
    if(!logicInstance) {
        LogWarning(errStr, StringFormat("Can't find logic with id '%d' in entity: '%s'",
             logicId, entity->ET_getName()));
        return false;
    }
    return true;
}

bool IsValidChildId(EntityChildId childId, const std::vector<Entity::EntityChildNode>& children) {
    if(childId == InvalidEntityChildId) {
        return false;
    }
    for(auto& childNode : children) {
        if(childId == childNode.childId) {
            return false;
        }
    }
    return true;
}

bool IsValidLogicId(EntityLogicId logicId, const std::vector<EntityLogicNode>& logics) {
    if(logicId == InvalidEntityLogicId) {
        return false;
    }
    for(auto& logicNode : logics) {
        if(logicId == logicNode.logicId) {
            return false;
        }
    }
    return true;
}

} // namespace

Entity::Entity(const char* entityName, EntityId entId) :
    name(entityName),
    entityId(entId) {

    assert(!name.empty() && "Invalid entity name");

    ETNode<ETEntity>::connect(getEntityId());
}

Entity::~Entity() {
    for(auto it = children.rbegin(), end = children.rend(); it != end; ++it) {
        ET_SendEvent(&ETEntityManager::ET_destroyEntity, it->childEntId);
    }
    for(auto it = logics.rbegin(), end = logics.rend(); it != end; ++it) {
        auto logicPtr = static_cast<EntityLogic*>(it->logic.get());
        logicPtr->deinit();
    }
}

void Entity::addChildEntityWithId(EntityChildId childId, Entity& childEntity) {
    assert(IsValidChildId(childId, children) && "Invalid children id");
    childEntity.parentId = entityId;

    Transform childTm = childEntity.tm;
    childTm.pt += tm.pt;
    childTm.scale.x *= tm.scale.x;
    childTm.scale.y *= tm.scale.y;
    childTm.scale.z *= tm.scale.z;
    childEntity.ET_setTransform(childTm);

    children.emplace_back(EntityChildNode{childEntity.entityId, childId});
}

EntityLogicId Entity::addLogic(ClassInstance&& logicInstance) {
    assert(logicInstance.get() && "Can't add null game logic");
    auto logicPtr = static_cast<EntityLogic*>(logicInstance.get());
    logicPtr->setEntity(this);
    if(!logicPtr->init()) {
        return InvalidEntityLogicId;
    }
    auto logicId = createNewLogicId();
    logics.emplace_back(std::move(logicInstance), logicId);
    return logicId;
}

bool Entity::addLogicWithId(EntityLogicId logicId, ClassInstance&& logicInstance) {
    assert(logicInstance.get() && "Can't add null game logic");
    assert(IsValidLogicId(logicId, logics) && "Invalid entity logic id");
    auto logicPtr = static_cast<EntityLogic*>(logicInstance.get());
    logicPtr->setEntity(this);
    if(!logicPtr->init()) {
        return false;
    }
    logics.emplace_back(std::move(logicInstance), logicId);
    return true;
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

bool Entity::readLogicData(EntityLogicId logicId, EntityLogicValueId valueId, MemoryStream& stream) {
    const char* errStr = "[Entity::readLogicData] Can't read logic data (Error: '%s')";
    auto logicInstance = findLogic(logicId);
    if(!CheckLogicValue(errStr, this, logicId, logicInstance, valueId)) {
        return false;
    }
    if(!logicInstance->writeValueTo(valueId, stream)) {
        LogWarning(errStr, StringFormat("Error during read from logic with id '%d' in entity: '%s'",
             logicId, ET_getName()));
        return false;
    }
    return true;
}

bool Entity::writeLogicData(EntityLogicId logicId, EntityLogicValueId valueId, MemoryStream& stream) {
    const char* errStr = "[Entity::writeLogicData] Can't write logic data (Error: '%s')";
    auto logicInstance = findLogic(logicId);
    if(!CheckLogicValue(errStr, this, logicId, logicInstance, valueId)) {
        return false;
    }
    bool res = true;
    auto logicPtr = static_cast<EntityLogic*>(logicInstance->get());
    logicPtr->deinit();
    if(!logicInstance->readValueFrom(valueId, stream)) {
        LogError(errStr, StringFormat("Error during write to logic with id '%d' in entity: '%s'",
             logicId, ET_getName()));
        res = false;
    }
    if(!logicPtr->init()) {
        LogWarning("[Entity::writeLogicData] Can't re-init logic after write");
    }
    return res;
}

bool Entity::addLogicValueArrayElemet(EntityLogicId logicId, EntityLogicValueId valueId) {
    const char* errStr = "[Entity::addLogicValueArrayElemet] Can't add logic array element (Error: '%s')";
    auto logicInstance = findLogic(logicId);
    if(!CheckLogicValue(errStr, this, logicId, logicInstance, valueId)) {
        return false;
    }
    bool res = true;
    auto logicPtr = static_cast<EntityLogic*>(logicInstance->get());
    logicPtr->deinit();
    if(!logicInstance->addValueArrayElement(valueId)) {
        res = false;
    }
    if(!logicPtr->init()) {
        LogWarning("[Entity::addLogicValueArrayElemet] Can't re-init logic after element add");
    }
    return res;
}

const char* Entity::ET_getName() const {
    return name.c_str();
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

EntityChildId Entity::ET_addChild(EntityId entId) {
    assert(entId != entityId && "Can't add self as a child");
    assert(entId != InvalidEntityId && "Invalid entity id");
    for(auto childNode : children) {
        if(childNode.childEntId == entId) {
            return InvalidEntityChildId;
        }
    }
    auto childId = createNewChildId();
    children.emplace_back(EntityChildNode{entId, childId});
    EntityId entParentId;
    ET_SendEventReturn(entParentId, entId, &ETEntity::ET_getParentId);
    if(entParentId != entityId) {
        ET_SendEvent(entId, &ETEntity::ET_setParent, entityId);
    }
    ET_SendEvent(entityId, &ETEntityEvents::ET_onChildAdded, entId);
    return childId;
}

void Entity::ET_removeChild(EntityId entId) {
    auto it = children.begin();
    for(auto end = children.end(); it != end; ++it) {
        if(it->childEntId == entId) {
            break;
        }
    }
    if(it != children.end()) {
        children.erase(it);
        EntityId entParentId;
        ET_SendEventReturn(entParentId, entId, &ETEntity::ET_getParentId);
        if(entParentId == entityId) {
            ET_SendEvent(entId, &ETEntity::ET_setParent, InvalidEntityId);
        }
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
    for(auto childNode : children) {
        Transform childTm;
        ET_SendEventReturn(childTm, childNode.childEntId, &ETEntity::ET_getTransform);
        childTm.pt += ptOffset;
        childTm.scale = Vec3(childTm.scale.x * scaleFactor.x,
            childTm.scale.y * scaleFactor.y,
            childTm.scale.z * scaleFactor.z);
        ET_SendEvent(childNode.childEntId, &ETEntity::ET_setTransform, childTm);
    }
    ET_SendEvent(entityId, &ETEntityEvents::ET_onTransformChanged, tm);
}

Transform Entity::ET_getLocalTransform() const {
    if(!parentId.isValid()) {
        return tm;
    }

    Transform parentTm;
    ET_SendEventReturn(parentTm, parentId, &ETEntity::ET_getTransform);

    Transform localTm;
    localTm.pt = tm.pt - parentTm.pt;
    localTm.scale.x = tm.scale.x / parentTm.scale.x;
    localTm.scale.y = tm.scale.y / parentTm.scale.y;
    localTm.scale.z = tm.scale.z / parentTm.scale.z;
    localTm.quat = Quat();
    return localTm;
}

void Entity::ET_setLocalTransform(const Transform& localTm) {
    if(!parentId.isValid()) {
        LogWarning("[Entity::ET_setLocalTransform] Can't set local transform to entity '%s' without parent",
            name);
        return;
    }

    Transform parentTm;
    ET_SendEventReturn(parentTm, parentId, &ETEntity::ET_getTransform);

    Transform newTm;
    newTm.pt = parentTm.pt + localTm.pt;
    newTm.scale.x = parentTm.scale.x * localTm.scale.x;
    newTm.scale.y = parentTm.scale.y * localTm.scale.y;
    newTm.scale.z = parentTm.scale.z * localTm.scale.z;
    newTm.quat = Quat();
    ET_setTransform(newTm);
}

int Entity::ET_getMaxChildrenDepth() const {
    int maxDepth = 0;
    for(auto childNode : children) {
        int childMaxDepth = 0;
        ET_SendEventReturn(childMaxDepth, childNode.childEntId, &ETEntity::ET_getMaxChildrenDepth);
        childMaxDepth++;
        if(childMaxDepth > maxDepth) {
            maxDepth = childMaxDepth;
        }
    }
    return maxDepth;
}

EntityLogicId Entity::createNewLogicId() const {
    EntityLogicId logicId = 1;
    for(auto& logicNode : logics) {
        if(logicNode.logicId >= logicId) {
            logicId = logicNode.logicId + 1;
        }
    }
    return logicId;
}

EntityChildId Entity::createNewChildId() const {
    EntityChildId childId = 1;
    for(auto& chilNode : children) {
        if(chilNode.childId >= childId) {
            childId = chilNode.childId + 1;
        }
    }
    return childId;
}

EntityChildId Entity::ET_getChildIdFromEntityId(EntityId childEntId) const {
    for(auto& chilNode : children) {
        if(chilNode.childEntId == childEntId) {
            return chilNode.childId;
        }
    }
    return InvalidEntityChildId;
}

EntityId Entity::ET_getEntityIdFromChildId(EntityChildId childId) const {
    for(auto& chilNode : children) {
        if(chilNode.childId == childId) {
            return chilNode.childEntId;
        }
    }
    return InvalidEntityId;
}