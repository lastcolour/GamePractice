#include "Entity.hpp"
#include "Entity/EntityLogic.hpp"
#include "Entity/ETEntityManager.hpp"
#include "Reflect/ClassInfoManager.hpp"
#include "EntityRegistry.hpp"

#include <cassert>

namespace {

bool CheckLogicValue(const char* errStr, Entity* entity, EntityLogicId logicId,
    Reflect::ClassInstance* logicInstance, Reflect::ClassValueId valueId) {
    assert(logicId != InvalidEntityLogicId && "Invalid logic id");
    assert(valueId != Reflect::InvalidClassValueId && "Invalid logic value id");
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

void InitDeinitAllLogic(EntityId entId, std::vector<Entity::EntityLogicNode>& logics, bool init) {
    if(init) {
        LogDebug("[InitDeinitAllLogic] Init all logic on entity: '%s'", EntityUtils::GetEntityName(entId));
        for(auto& logicNode : logics) {
            auto logicPtr = static_cast<EntityLogic*>(logicNode.logic.get());
            logicPtr->init();
        }
        for(auto& logicNode : logics) {
            auto logicPtr = static_cast<EntityLogic*>(logicNode.logic.get());
            logicPtr->onLoaded();
        }
    } else {
        LogDebug("[InitDeinitAllLogic] Deinit all logic on entity: '%s'", EntityUtils::GetEntityName(entId));
        for(auto& logicNode : logics) {
            auto logicPtr = static_cast<EntityLogic*>(logicNode.logic.get());
            logicPtr->deinit();
        }
    }
}

} // namespace

Entity::Entity(const char* entityName, EntityRegistry* entityRegistry, EntityId entId) :
    registry(entityRegistry),
    parent(nullptr),
    name(StringFormat("%s:%d", entityName, entId.getRawId())),
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
    children.emplace_back(EntityChildNode{&childEntity, childId});
}

EntityLogicId Entity::addLogic(Reflect::ClassInstance&& logicInstance) {
    assert(logicInstance.get() && "Can't add null game logic");
    auto logicPtr = static_cast<EntityLogic*>(logicInstance.get());
    logicPtr->entity = this;
    logicPtr->init();
    auto logicId = createNewLogicId();
    logics.emplace_back(EntityLogicNode{std::move(logicInstance), logicId});
    logicPtr->onLoaded();
    return logicId;
}

bool Entity::addLogicWithId(EntityLogicId logicId, Reflect::ClassInstance&& logicInstance) {
    assert(logicInstance.get() && "Can't add null game logic");
    assert(IsValidLogicId(logicId, logics) && "Invalid entity logic id");
    auto logicPtr = static_cast<EntityLogic*>(logicInstance.get());
    logicPtr->entity = this;
    logicPtr->init();
    logics.emplace_back(EntityLogicNode{std::move(logicInstance), logicId});
    return true;
}

void* Entity::addLogicByTypeId(Core::TypeId logicTypeId) {
    auto classInfo = GetEnv()->GetClassInfoManager()->findClassInfoByTypeId(logicTypeId);
    if(!classInfo) {
        return nullptr;
    }
    auto instance = classInfo->createInstance();
    auto logicPtr = instance.get();
    if(!logicPtr) {
        LogError("[Entity::addLogicByTypeId] Can't create instance of logic with type id: '%d'", logicTypeId);
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

Reflect::ClassInstance* Entity::findLogic(EntityLogicId logicId) {
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

bool Entity::readLogicData(EntityLogicId logicId, Reflect::ClassValueId valueId, Memory::MemoryStream& stream) {
    const char* errStr = "[Entity::readLogicData] Can't read logic data (Error: '%s')";
    auto logicInstance = findLogic(logicId);
    if(!CheckLogicValue(errStr, this, logicId, logicInstance, valueId)) {
        return false;
    }
    Reflect::SerializeContext serCtx;
    serCtx.entityId = entityId;
    if(!logicInstance->writeValueTo(serCtx, valueId, stream)) {
        LogWarning(errStr, StringFormat("Error during read from logic with id '%d' in entity: '%s'",
             logicId, ET_getName()));
        return false;
    }
    return true;
}

bool Entity::writeLogicData(EntityLogicId logicId, Reflect::ClassValueId valueId, Memory::MemoryStream& stream) {
    const char* errStr = "[Entity::writeLogicData] Can't write logic data (Error: '%s')";
    auto logicInstance = findLogic(logicId);
    if(!CheckLogicValue(errStr, this, logicId, logicInstance, valueId)) {
        return false;
    }
    InitDeinitAllLogic(getEntityId(), logics, false);
    bool res = true;
    Reflect::SerializeContext serCtx;
    serCtx.entityId = entityId;
    if(!logicInstance->readValueFrom(serCtx, valueId, stream)) {
        LogError(errStr, StringFormat("Error during write to logic with id '%d' in entity: '%s'",
             logicId, ET_getName()));
        res = false;
    }
    InitDeinitAllLogic(getEntityId(), logics, true);
    return res;
}

bool Entity::addLogicValueArrayElemet(EntityLogicId logicId, Reflect::ClassValueId valueId) {
    const char* errStr = "[Entity::addLogicValueArrayElemet] Can't add logic array element (Error: '%s')";
    auto logicInstance = findLogic(logicId);
    if(!CheckLogicValue(errStr, this, logicId, logicInstance, valueId)) {
        return false;
    }
    InitDeinitAllLogic(getEntityId(), logics, false);
    bool res = true;
    if(!logicInstance->addValueArrayElement(valueId)) {
        res = false;
    }
    InitDeinitAllLogic(getEntityId(), logics, true);
    return res;
}

bool Entity::setLogicValuePolymorphType(EntityLogicId logicId, Reflect::ClassValueId valueId, const char* typeName) {
    const char* errStr = "[Entity::setLogicValuePolymorphType] Can't set polymorph type (Error: '%s')";
    auto logicInstance = findLogic(logicId);
    if(!CheckLogicValue(errStr, this, logicId, logicInstance, valueId)) {
        return false;
    }
    InitDeinitAllLogic(getEntityId(), logics, false);
    bool res = true;
    if(!logicInstance->setValuePolymorphType(valueId, typeName)) {
        res = false;
    }
    InitDeinitAllLogic(getEntityId(), logics, true);
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
            if(parent) {
                parent->removeChild(this);
            }
            return;
        }
    }
    if(newParent) {
        newParent->addChild(this);
    }
}

EntityChildId Entity::ET_addChild(EntityId entId) {
    if(!entId.isValid()) {
        LogError("[Entity::ET_addChild] Can't add invalid entity as a child to entity: '%s'", name);
        return InvalidEntityChildId;
    }
    assert(entId != entityId && "Can't add self as a child");

    auto childEntity = registry->findEntity(entId);

    if(!childEntity) {
        LogError("[Entity::ET_addChild] Can't find entity with id '%d' to add as a child to: '%s'",
            entId.getRawId(), name);
        return InvalidEntityChildId;
    }

    return addChild(childEntity);
}

EntityChildId Entity::addChild(Entity* childEntity) {
    for(auto childNode : children) {
        if(childNode.childEntity == childEntity) {
            return InvalidEntityChildId;
        }
    }

    auto childId = createNewChildId();
    children.emplace_back(EntityChildNode{childEntity, childId});

    if(childEntity->parent) {
        childEntity->parent->removeChild(childEntity);
    }

    childEntity->parent = this;

    return childId;
}

void Entity::removeChild(Entity* childEntity) {
    assert(childEntity->parent == this && "Invalid parent");
    childEntity->parent = nullptr;
    for(auto it = children.begin(), end = children.end(); it != end; ++it) {
        if(it->childEntity == childEntity) {
            children.erase(it);
            return;
        }
    }
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
        assert(childEntity->parent == this && "Invalid parent");
        childEntity->parent = nullptr;
        children.erase(it);
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
    for(auto& childNode : children) {
        auto childEntity = childNode.childEntity;

        auto currLocal = tm.getLocalDelta(childEntity->tm);
        auto newChildTm = newTm;
        newChildTm.addLocalDelta(currLocal);

        childEntity->ET_setTransform(newChildTm);
    }
    tm = newTm;
    tm.scale.x = std::max(0.001f, newTm.scale.x);
    tm.scale.y = std::max(0.001f, newTm.scale.y);
    tm.scale.z = std::max(0.001f, newTm.scale.z);
    for(auto& logicNode : logics) {
        auto logicPtr = static_cast<EntityLogic*>(logicNode.logic.get());
        logicPtr->onTransformChanged(tm);
    }
}

Transform Entity::ET_getLocalTransform() const {
    if(!parent) {
        return tm;
    }

    return parent->tm.getLocalDelta(tm);
}

void Entity::ET_setLocalTransform(const Transform& newLocalTm) {
    if(!parent) {
        ET_setTransform(newLocalTm);
        return;
    }

    Transform newTm = parent->tm;
    newTm.addLocalDelta(newLocalTm);
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

EntityId Entity::ET_getEntityIdFromChildId(const std::vector<EntityChildId>& childrenIds) const {
    if(childrenIds.empty()) {
        return InvalidEntityId;
    }
    const Entity* currEntity = this;
    for(auto childId : childrenIds) {
        if(childId == 0) {
            assert(childrenIds.back() == 0 && "Invalid chilrend id sequence. (Self referencing should be last)");
            return currEntity->getEntityId();
        }
        bool foundChild = false;
        for(auto& childNode : currEntity->children) {
            if(childNode.childId == childId) {
                currEntity = childNode.childEntity;
                foundChild = true;
                break;
            }
        }
        if(!foundChild) {
            LogError("[Entity::ET_getEntityIdFromChildId] Can't find child with id '%d' on entity '%s'",
                childId, currEntity->name);
            return InvalidEntityId;
        }
    }
    return currEntity->getEntityId();
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

void Entity::setLoaded() {
    for(auto& logicNode : logics) {
        auto logicPtr = static_cast<EntityLogic*>(logicNode.logic.get());
        logicPtr->onLoaded();
    }
}

std::vector<EntityLogic*> Entity::ET_getLogisByTypeId(Core::TypeId logicTypeId) {
    std::vector<EntityLogic*> result;
    for(auto& node : logics) {
        if(node.logic.getInstanceTypeId() == logicTypeId) {
            auto ptr = static_cast<EntityLogic*>(node.logic.get());
            result.push_back(ptr);
        }
    }
    return result;
}