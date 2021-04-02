#include "Entity.hpp"
#include "Entity/EntityLogic.hpp"
#include "Entity/ETEntityManager.hpp"
#include "Reflect/ETReflectInterfaces.hpp"
#include "EntityRegistry.hpp"

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
    if(childId == 0) {
        return false;
    }
    for(auto& childNode : children) {
        if(childId == childNode.childId) {
            return false;
        }
    }
    return true;
}

bool IsValidLogicId(EntityLogicId logicId, const std::vector<Entity::EntityLogicNode>& logics) {
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

Entity::Entity(const char* entityName, EntityRegistry* entityRegistry, EntityId entId) :
    registry(entityRegistry),
    parent(nullptr),
    name(entityName),
    entityId(entId) {

    assert(!name.empty() && "Invalid entity name");

    ETNode<ETEntity>::connect(getEntityId());
}

Entity::~Entity() {
    if(parent) {
        parent->ET_removeChild(getEntityId());
    }
    auto entToRemove = std::move(children);
    for(auto it = entToRemove.rbegin(), end = entToRemove.rend(); it != end; ++it) {
        registry->removeEntity(it->childEntity);
    }
    for(auto it = logics.rbegin(), end = logics.rend(); it != end; ++it) {
        auto logicPtr = static_cast<EntityLogic*>(it->logic.get());
        logicPtr->deinit();
    }
}

void Entity::addChildEntityWithId(EntityChildId childId, Entity& childEntity) {
    assert(IsValidChildId(childId, children) && "Invalid children id");
    childEntity.parent = this;

    Transform childTm = childEntity.tm;
    childTm.pt += tm.pt;
    childTm.scale.x *= tm.scale.x;
    childTm.scale.y *= tm.scale.y;
    childTm.scale.z *= tm.scale.z;
    childEntity.ET_setTransform(childTm);

    children.emplace_back(EntityChildNode{&childEntity, childId});
}

EntityLogicId Entity::addLogic(ClassInstance&& logicInstance) {
    assert(logicInstance.get() && "Can't add null game logic");
    auto logicPtr = static_cast<EntityLogic*>(logicInstance.get());
    logicPtr->setEntity(this);
    logicPtr->init();
    auto logicId = createNewLogicId();
    logics.emplace_back(EntityLogicNode{std::move(logicInstance), logicId});
    ET_SendEvent(getEntityId(), &ETEntityEvents::ET_onLoaded);
    return logicId;
}

bool Entity::addLogicWithId(EntityLogicId logicId, ClassInstance&& logicInstance) {
    assert(logicInstance.get() && "Can't add null game logic");
    assert(IsValidLogicId(logicId, logics) && "Invalid entity logic id");
    auto logicPtr = static_cast<EntityLogic*>(logicInstance.get());
    logicPtr->setEntity(this);
    logicPtr->init();
    logics.emplace_back(EntityLogicNode{std::move(logicInstance), logicId});
    return true;
}

void* Entity::addLogicByTypeId(TypeId logicTypeId) {
    ClassInfo* classInfo = nullptr;
    ET_SendEventReturn(classInfo, &ETClassInfoManager::ET_findClassInfoByTypeId, logicTypeId);
    if(!classInfo) {
        LogWarning("[Entity::addLogicByTypeId] Can't find logic with type id: '%d'", logicTypeId);
        return nullptr;
    }
    auto instance = classInfo->createInstance();
    auto logicPtr = instance.get();
    if(!logicPtr) {
        LogWarning("[Entity::addLogicByTypeId] Can't create instance of logic with type id: '%d'", logicTypeId);
        return nullptr;
    }
    addLogic(std::move(instance));
    return logicPtr;
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
    auto logicPtr = static_cast<EntityLogic*>(it->logic.get());
    logicPtr->deinit();
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

void Entity::setName(const char* newName) {
    assert(newName && newName[0]);
    name = newName;
}

bool Entity::readLogicData(EntityLogicId logicId, EntityLogicValueId valueId, MemoryStream& stream) {
    const char* errStr = "[Entity::readLogicData] Can't read logic data (Error: '%s')";
    auto logicInstance = findLogic(logicId);
    if(!CheckLogicValue(errStr, this, logicId, logicInstance, valueId)) {
        return false;
    }
    SerializeContext serCtx;
    serCtx.entityId = entityId;
    if(!logicInstance->writeValueTo(serCtx, valueId, stream)) {
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

    SerializeContext serCtx;
    serCtx.entityId = entityId;
    if(!logicInstance->readValueFrom(serCtx, valueId, stream)) {
        LogError(errStr, StringFormat("Error during write to logic with id '%d' in entity: '%s'",
             logicId, ET_getName()));
        res = false;
    }
    logicPtr->init();
    ET_SendEvent(getEntityId(), &ETEntityEvents::ET_onLoaded);

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
    logicPtr->init();
    ET_SendEvent(getEntityId(), &ETEntityEvents::ET_onLoaded);

    return res;
}

const char* Entity::ET_getName() const {
    return name.c_str();
}

void Entity::ET_setParent(EntityId entId) {
    assert(entId != entityId && "Can't set self as a parent");
    if(parent && parent->getEntityId() == entId) {
        return;
    }
    Entity* newParent = nullptr;
    if(entId.isValid()) {
        newParent = registry->findEntity(entId);
        if(!newParent) {
            LogWarning("[Entity::ET_setParent] Can't find entity with id '%d' to set as a parent for: '%s'",
                entId.getRawId(), name);
        }
    }
    if(parent) {
        parent->ET_removeChild(getEntityId());
    }
    parent = newParent;
    if(parent) {
        parent->ET_addChild(getEntityId());
    }
}

EntityChildId Entity::ET_addChild(EntityId entId) {
    assert(entId != entityId && "Can't add self as a child");
    assert(entId != InvalidEntityId && "Invalid entity id");

    auto childEntity = registry->findEntity(entId);
    if(!childEntity) {
        LogError("[Entity::ET_addChild] Can't find entity with id '%d' to add as a child to: '%s'",
            entId.getRawId(), name);
        return InvalidEntityChildId;
    }

    for(auto childNode : children) {
        if(childNode.childEntity == childEntity) {
            return InvalidEntityChildId;
        }
    }

    auto childId = createNewChildId();
    children.emplace_back(EntityChildNode{childEntity, childId});

    if(childEntity->ET_getParentId() != getEntityId()) {
        childEntity->ET_setParent(getEntityId());
    }
    return childId;
}

void Entity::ET_removeChild(EntityId entId) {
    auto it = children.begin();
    for(auto end = children.end(); it != end; ++it) {
        if(it->childEntity->getEntityId() == entId) {
            break;
        }
    }
    if(it != children.end()) {
        auto childEntity = it->childEntity;
        children.erase(it);
        if(childEntity->ET_getParentId() == getEntityId()) {
            childEntity->ET_setParent(InvalidEntityId);
        }
    }
}

EntityId Entity::ET_getParentId() const {
    if(parent) {
        return parent->getEntityId();
    }
    return InvalidEntityId;
}

const Transform& Entity::ET_getTransform() const {
    return tm;
}

void Entity::ET_setTransform(const Transform& newTm) {
    Vec3 newScale = newTm.scale;
    newScale.x = std::max(0.0001f, newScale.x);
    newScale.y = std::max(0.0001f, newScale.y);
    newScale.z = std::max(0.0001f, newScale.z);

    for(auto& childNode : children) {
        auto childEntity = childNode.childEntity;

        Transform childTm = childEntity->ET_getLocalTransform();
        Vec3 shift = childTm.pt.getScaled(newScale);
        shift = newTm.quat * shift;
        childTm.pt = newTm.pt + shift;
        childTm.scale.scale(newScale);
        childTm.quat *= newTm.quat;

        childEntity->ET_setTransform(childTm);
    }

    tm.pt = newTm.pt;
    tm.scale = newScale;
    tm.quat = newTm.quat;

    ET_SendEvent(entityId, &ETEntityEvents::ET_onTransformChanged, tm);
}

Transform Entity::ET_getLocalTransform() const {
    if(!parent) {
        return tm;
    }

    Transform parentTm = parent->ET_getTransform();
    Quat qInv = parentTm.quat.getInversed();

    Transform localTm;
    localTm.pt = tm.pt - parentTm.pt;
    localTm.pt = qInv * localTm.pt;
    localTm.pt.x /= parentTm.scale.x;
    localTm.pt.y /= parentTm.scale.y;
    localTm.pt.z /= parentTm.scale.z;

    localTm.scale.x = tm.scale.x / parentTm.scale.x;
    localTm.scale.y = tm.scale.y / parentTm.scale.y;
    localTm.scale.z = tm.scale.z / parentTm.scale.z;

    localTm.quat = qInv * tm.quat;
    return localTm;
}

void Entity::ET_setLocalTransform(const Transform& newLocalTm) {
    if(!parent) {
        ET_setTransform(newLocalTm);
        return;
    }

    Vec3 newScale = newLocalTm.scale;
    newScale.x = std::max(0.0001f, newScale.x);
    newScale.y = std::max(0.0001f, newScale.y);
    newScale.z = std::max(0.0001f, newScale.z);

    Transform parentTm = parent->ET_getTransform();

    Vec3 shift = newLocalTm.pt;
    shift.x *= parentTm.scale.x;
    shift.y *= parentTm.scale.y;
    shift.z *= parentTm.scale.z;

    Transform newTm;
    newTm.pt = parentTm.pt + parentTm.quat * shift;
    newTm.scale = newScale.getScaled(parentTm.scale);
    newTm.quat = newLocalTm.quat * parentTm.quat;

    ET_setTransform(newTm);
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
    if(childEntId == entityId) {
        return 0;
    }
    for(auto& chilNode : children) {
        if(chilNode.childEntity->getEntityId() == childEntId) {
            return chilNode.childId;
        }
    }
    return InvalidEntityChildId;
}

EntityId Entity::ET_getEntityIdFromChildId(EntityChildId childId) const {
    if(childId == 0) {
        return entityId;
    }
    for(auto& chilNode : children) {
        if(chilNode.childId == childId) {
            return chilNode.childEntity->getEntityId();
        }
    }
    return InvalidEntityId;
}

std::vector<EntityId> Entity::ET_getChildren() const {
    std::vector<EntityId> res;
    res.reserve(children.size());
    for(auto& childNode : children) {
        res.push_back(childNode.childEntity->getEntityId());
    }
    return res;
}

void Entity::purgeAllRelationships() {
    parent = nullptr;
    children.clear();
}

std::vector<EntityLogic*> Entity::ET_getLogisByTypeId(TypeId logicTypeId) {
    std::vector<EntityLogic*> result;
    for(auto& node : logics) {
        if(node.logic.getInstanceTypeId() == logicTypeId) {
            auto ptr = static_cast<EntityLogic*>(node.logic.get());
            result.push_back(ptr);
        }
    }
    return result;
}