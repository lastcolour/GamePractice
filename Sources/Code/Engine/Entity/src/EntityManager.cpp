#include "EntityManager.hpp"
#include "Entity.hpp"
#include "Entity/EntityLogic.hpp"
#include "Entity/EntityLogicsRegister.hpp"
#include "Core/ETAssets.hpp"
#include "Reflect/ClassInfoManager.hpp"
#include "Core/TimePoint.hpp"

#include <cassert>

namespace {

const EntityLogicId TransformLogicId = 0;
const float ENTITY_LOADING_TIME_LOG_THRESHOLD = 5.f;

bool CheckEntity(const char* errStr, EntityId entityId, Entity* entity) {
    if(entityId == InvalidEntityId) {
        LogWarning(errStr, "Invalid entity id");
        return false;
    }
    if(!entity) {
        LogWarning(errStr, StringFormat("Can't find entity with id: '%d'", entityId.getRawId()));
        return false;
    }
    return true;
}

bool CheckEntityLogic(const char* errStr, EntityId entityId, Entity* entity, EntityLogicId logicId) {
    if(!CheckEntity(errStr, entityId, entity)) {
        return false;
    }
    if(logicId == InvalidEntityLogicId) {
        LogWarning(errStr, StringFormat("Invalid logic id for entity: '%s'", entity->ET_getName()));
        return false;
    }
    return true;
}

bool CheckEntityLogicValue(const char* errStr, EntityId entityId, Entity* entity, EntityLogicId logicId, Reflect::ClassValueId valueId) {
    if(!CheckEntityLogic(errStr, entityId, entity, logicId)) {
        return false;
    }
    if(valueId == Reflect::InvalidClassValueId) {
        LogWarning(errStr, StringFormat("Invalid value id: '%d' of logic with id '%d' for entity: '%s'",
            valueId, logicId, entity->ET_getName()));
        return false;
    }
    return true;
}

bool isInsideAsyncLoad() {
    bool res = false;
    ET_SendEventReturn(res, &ETAsyncEntityManager::ET_isInsideAsyncLoad);
    return res;
}

} // namespace

EntityManager::EntityManager() :
    tmClassInfo(nullptr) {
}

EntityManager::~EntityManager() {
    ET_destroyAllEntities();
}

bool EntityManager::init() {
    ETNode<ETEntityManager>::connect(getEntityId());
    ReflectContext ctx;
    if(!ctx.reflect<Transform>()) {
        return false;
    }
    tmClassInfo = ctx.getRegisteredClassInfo();
    if(!tmClassInfo) {
        return false;
    }
    return true;
}

void EntityManager::deinit() {
    ETNode<ETEntityManager>::disconnect();
}

EntityId EntityManager::ET_createEntity(const char* entityName) {
    bool insideAsyncLoad = isInsideAsyncLoad();
    bool finishLoad = !insideAsyncLoad;

    auto entity = createEntity(entityName, finishLoad);
    if(!entity) {
        LogWarning("[EntityManager::ET_createEntity] Can't create entity from: '%s'", entityName);
        return InvalidEntityId;
    }

    if(insideAsyncLoad) {
        ET_SendEvent(&ETAsyncEntityManager::ET_addEntityToFinishLater, entity->getEntityId());
    }

    return entity->getEntityId();
}

EntityId EntityManager::ET_createEntityFromData(const char* entityName, const char* entityData) {
    auto node = JSONNode::ParseString(entityData);
    if(!node) {
        LogError("[EntityManager::ET_createEntityFromData] Can't parse data to create an entity: '%s'", entityName);
        return InvalidEntityId;
    }
    return ET_createEntityFromJSON(node, entityName);
}

EntityChildId EntityManager::ET_createChildEntity(EntityId parentId, const char* entityName) {
    const char* errStr = "[EntityManager::ET_createChildEntity] Can't create child entity (Error: %s)";
    auto parentEntity = registry.findEntity(parentId);
    if(!CheckEntity(errStr, parentId, parentEntity)) {
        return InvalidEntityChildId;
    }
    if(!entityName || !entityName[0]) {
        LogWarning(errStr, "Empty child name");
        return InvalidEntityChildId;
    }

    EntityId childEntityId;
    {
        auto childEntity = registry.createEntity(entityName);
        if(!childEntity) {
            assert(false && "Can't create entity");
            return InvalidEntityChildId;
        }
        auto& tm = parentEntity->ET_getTransform();
        childEntity->ET_setTransform(tm);
        childEntityId = childEntity->getEntityId();
    }

    auto childId = parentEntity->ET_addChild(childEntityId);
    if(childId == InvalidEntityChildId) {
        LogWarning(errStr, "Can't add child");
        return InvalidEntityChildId;
    }
    return childId;
}

bool EntityManager::ET_renameEntity(EntityId entId, const char* newName) {
    const char* errStr = "[EntityManager::ET_renameEntity] Can't rename entity (Error: %s)";
    auto entity = registry.findEntity(entId);
    if(!CheckEntity(errStr, entId, entity)) {
        return false;
    }
    if(!newName || !newName[0]) {
        LogWarning(errStr, StringFormat("Can't rename entity '%s' to '%s'", entity->ET_getName()));
        return false;
    }
    entity->setName(newName);
    return true;
}

void EntityManager::ET_destroyEntity(EntityId targetEntId) {
    registry.removeEntity(targetEntId);
}

void EntityManager::ET_destroyAllEntities() {
    registry.removeAllEntities();
}

bool EntityManager::ET_registerLogics(EntityLogicsRegister& logicsRegister) {
    for(auto classInfo : logicsRegister.getLogicClasses()) {
        assert(classInfo && "Invalid logic class info");
        auto it = registeredLogics.find(classInfo->getName());
        if(it != registeredLogics.end()) {
            LogError("[EntityManager::ET_registerLogics] Logic already registered with such name: '%s'", classInfo->getName());
            return false;
        }
        registeredLogics[classInfo->getName()] = { logicsRegister.getModuleName(), classInfo };
    }
    return true;
}

EntityId EntityManager::ET_createEntityFromJSON(const JSONNode& node, const char* entityName) {
    if(!entityName || !entityName[0]) {
        LogWarning("[EntityManager::ET_createEntityFromJSON] Can't create entity with empty name");
        return InvalidEntityId;
    }
    if(!node) {
        LogWarning("[EntityManager::ET_createEntityFromJSON] Invalid JSON node to create entity '%s'", entityName);
        return InvalidEntityId;
    }

    auto loadStartT = TimePoint::GetNow();

    bool finishLoad = true;
    auto entity = createEntityImpl(node, entityName, finishLoad);
    if(!entity) {
        LogWarning("[EntityManager::ET_createEntityFromJSON] Can't create entity from JSON node with name: '%s'", entityName);
        return InvalidEntityId;
    }

    float msValue = loadStartT.getMsDeltaWithNow();
    if(msValue > ENTITY_LOADING_TIME_LOG_THRESHOLD) {
        LogDebug("[EntityManager::ET_createEntityFromJSON] Create entity: '%s' (%.1f ms)", entityName, msValue);
    }

    return entity->getEntityId();
}

EntityLogicId EntityManager::ET_addLogicToEntity(EntityId targetEntId, const char* logicName) {
    const char* errStr = "[EntityManager::ET_addLogicToEntity] Can't add logic (Error: %s)";
    auto entity = registry.findEntity(targetEntId);
    if(!CheckEntity(errStr, targetEntId, entity)) {
        return InvalidEntityLogicId;
    }
    if(!logicName || !logicName[0]) {
        LogWarning(errStr, StringFormat("Can't add logic with empty name for entity: '%s'", entity->ET_getName()));
        return InvalidEntityLogicId;
    }
    auto logicIt = registeredLogics.find(logicName);
    if(logicIt == registeredLogics.end()) {
        LogWarning(errStr, StringFormat("Can't find logic: '%s' for entity: '%s'", logicName, entity->ET_getName()));
        return InvalidEntityLogicId;
    }
    auto logicInstance = logicIt->second.classInfo->createInstance();
    if(!logicInstance.get()) {
        LogWarning(errStr, StringFormat("Can't create instance of logic: '%s' for entity: '%s'", logicName, entity->ET_getName()));
        return InvalidEntityLogicId;
    }
    auto logicId = entity->addLogic(std::move(logicInstance));
    if(logicId == InvalidEntityLogicId) {
        LogWarning(errStr, StringFormat("Can't init instance of logic: '%s' for entity: '%s'", logicName, entity->ET_getName()));
        return InvalidEntityLogicId;
    }
    LogDebug("[EntityManager::ET_addLogicToEntity] Add logic '%s' to entity '%s'", logicName, entity->ET_getName());
    return logicId;
}

void EntityManager::ET_removeLogicFromEntity(EntityId targetEntId, EntityLogicId logicId) {
    const char* errStr = "[EntityManager::ET_removeLogicFromEntity] Can't remove logic (Error: %s)";
    auto entity = registry.findEntity(targetEntId);
    if(!CheckEntityLogic(errStr, targetEntId, entity, logicId)) {
        return;
    }
    if(!entity->removeLogic(logicId)) {
        LogWarning(errStr, StringFormat("Can't find logic with id: '%d' for entity: '%s'", logicId, entity->ET_getName()));
        return;
    }
    LogDebug("[EntityManager::ET_removeLogicFromEntity] Remove logic with id '%d' from entity '%s'",
        logicId, entity->ET_getName());
}

bool EntityManager::ET_readEntityLogicData(EntityId targetEntId, EntityLogicId logicId,
    Reflect::ClassValueId valueId, Memory::MemoryStream& stream) {
    const char* errStr = "[EntityManager::ET_readEntityLogicData] Can't read logic data (Error: %s)";
    auto entity = registry.findEntity(targetEntId);
    if(!CheckEntityLogicValue(errStr, targetEntId, entity, logicId, valueId)) {
        return false;
    }
    if(logicId == TransformLogicId) {

        Reflect::SerializeContext serCtx;
        serCtx.entityId = entity->getEntityId();

        auto localTm = entity->ET_getLocalTransform();
        if(!tmClassInfo->writeValueTo(serCtx, &localTm, valueId, stream)) {
            LogWarning(errStr, StringFormat("Can't read transform data from entity: '%s'", entity->ET_getName()));
            return false;
        }
        return true;
    }
    return entity->readLogicData(logicId, valueId, stream);
}

bool EntityManager::ET_writeEntityLogicData(EntityId targetEntId, EntityLogicId logicId,
    Reflect::ClassValueId valueId, Memory::MemoryStream& stream) {
    const char* errStr = "[EntityManager::ET_writeEntityLogicData] Can't write logic data (Error: %s)";
    auto entity = registry.findEntity(targetEntId);
    if(!CheckEntityLogicValue(errStr, targetEntId, entity, logicId, valueId)) {
        return false;
    }
    if(logicId == TransformLogicId) {
        Transform localTm = entity->ET_getLocalTransform();
        Reflect::SerializeContext serCtx;
        serCtx.entityId = targetEntId;
        if(!tmClassInfo->readValueFrom(serCtx, &localTm, valueId, stream)) {
            LogWarning(errStr, StringFormat("Can't read transform data for entity: '%s'", entity->ET_getName()));
            return false;
        }
        entity->ET_setLocalTransform(localTm);
        return true;
    }
    return entity->writeLogicData(logicId, valueId, stream);
}

bool EntityManager::ET_addEntityLogicArrayElement(EntityId targetEntId, EntityLogicId logicId,
    Reflect::ClassValueId valueId) {
    const char* errStr = "[EntityManager::ET_addEntityLogicArrayElement] Can't add entity logic array element (Error: %s)";
    auto entity = registry.findEntity(targetEntId);
    if(!CheckEntityLogicValue(errStr, targetEntId, entity, logicId, valueId)) {
        return false;
    }
    if(logicId == TransformLogicId) {
        LogWarning("[EntityManager::ET_addEntityLogicArrayElement] Can't add array element to transform logic to entity: '%s'",
            entity->ET_getName());
        return false;
    }
    if(!entity->addLogicValueArrayElemet(logicId, valueId)) {
        return false;
    }
    return true;
}

bool EntityManager::ET_setEntityLogicPolymorphObjectType(EntityId targetEntId, EntityLogicId logicId,
    Reflect::ClassValueId valueId, const char* typeName) {
    const char* errStr = "[EntityManager::ET_setEntityLogicPolymorphObjectType] Can't update polymorph type of entity logic value (Error: %s)";
    auto entity = registry.findEntity(targetEntId);
    if(!CheckEntityLogicValue(errStr, targetEntId, entity, logicId, valueId)) {
        return false;
    }
    if(logicId == TransformLogicId) {
        LogWarning("[EntityManager::ET_setEntityLogicPolymorphObjectType] Can't set polymorph type for a transform logic to entity: '%s'",
            entity->ET_getName());
        return false;
    }
    if(!entity->setLogicValuePolymorphType(logicId, valueId, typeName)) {
        return false;
    }
    return true;
}

bool EntityManager::setupEntityLogics(Entity* entity, const JSONNode& node) const {
    auto logicsNodes = node.object("logics");
    if(!logicsNodes || logicsNodes.size() == 0u) {
        return true;
    }

    Reflect::SerializeContext serCtx;
    serCtx.entityId = entity->getEntityId();

    for(const auto& logicNode : logicsNodes) {
        if(!logicNode.hasKey("type")) {
            LogWarning("[EntityManager::setupEntityLogics] Can't find required logic 'type' data for an entity '%s'",
                entity->ET_getName());
            continue;
        }
        std::string logicType;
        logicNode.read("type", logicType);
        if(!logicNode.hasKey("id")) {
            LogWarning("[EntityManager::setupEntityLogics] Can't find required logic 'id' in logics data '%s for an entity '%s'",
                logicType, entity->ET_getName());
            continue;
        }
        auto it = registeredLogics.find(logicType);
        if(it == registeredLogics.end()) {
            LogError("[EntityManager::setupEntityLogics] Can't find logic type '%s' for entity '%s'", logicType, entity->ET_getName());
            continue;
        }
        EntityLogicId logicId = InvalidEntityLogicId;
        logicNode.read("id", logicId);
        if(logicId == InvalidEntityLogicId) {
            LogWarning("[EntityManager::setupEntityLogics] Invalid logic id for logic '%s' in entity '%s'", logicType, entity->ET_getName());
            continue;
        } else if (logicId == TransformLogicId) {
            LogWarning("[EntityManager::setupEntityLogics] Invalid logid id for logic '%s' in entity '%s'. (Error: logic id '0' is reserver for 'transform' logic)",
                logicType, entity->ET_getName());
            continue;
        }
        auto& logicClassInfo = *(it->second.classInfo);
        auto logicData = logicNode.object("data");
        if(!logicData) {
            LogWarning("[EntityManager::setupEntityLogics] Can't find logic data object for '%s' for entity '%s'", logicType, entity->ET_getName());
            continue;
        }
        auto logicInstance = logicClassInfo.createInstance();
        if(!logicInstance.get()) {
            LogWarning("[EntityManager::setupEntityLogics] Can't create instance of logic type '%s' for entity '%s'", logicType, entity->ET_getName());
            continue;
        }
        if(!logicInstance.readAllValuesFrom(serCtx, logicData)) {
            LogWarning("[EntityManager::setupEntityLogics] Can't read logic data of type '%s' for entity '%s'", logicType, entity->ET_getName());
            continue;
        }
        if(!entity->addLogicWithId(logicId, std::move(logicInstance))) {
            LogWarning("[EntityManager::setupEntityLogics] Can't init logic '%s' for entity '%s'", logicType, entity->ET_getName());
            continue;
        }
    }
    return true;
}

bool EntityManager::setupEntityChildren(Entity* entity, const JSONNode& node, bool finishLoad) {
    auto childrenNode = node.object("children");
    if(!childrenNode) {
        LogWarning("[EntityManager::setupEntityChildren] Can't find required 'children' node in entity '%s'", entity->ET_getName());
        return true;
    }
    for(const auto& childNode : childrenNode) {
        if(!childNode.hasKey("id")) {
            LogWarning("[EntityManager::setupEntityChildren] Can't find required 'id' value in entity '%s'", entity->ET_getName());
            continue;
        }
        if(!childNode.hasKey("name")) {
            LogWarning("[EntityManager::setupEntityChildren] Can't find required 'name' value in entity '%s'", entity->ET_getName());
            continue;
        }
        auto tmNode = childNode.object("transform");
        if(!tmNode) {
            LogWarning("[EntityManager::setupEntityChildren] Can't find required 'transform' node in entity '%s'", entity->ET_getName());
            continue;
        }
        if(!childNode.hasKey("internal")) {
            LogWarning("[EntityManager::setupEntityChildren] Can't find required 'internal' node in entity '%s'", entity->ET_getName());
            continue;
        }
        std::string childEntName;
        childNode.read("name", childEntName);

        bool isInternal = false;
        childNode.read("internal", isInternal);

        EntityChildId childId = InvalidEntityChildId;
        childNode.read("id", childId);
        if(childId == InvalidEntityChildId) {
            LogWarning("[EntityManager::setupEntityChildren] Invalid child id of child '%s' for an entity '%s'", childEntName, entity->ET_getName());
            continue;
        }

        Entity* childEntity = nullptr;
        if(isInternal) {
            JSONNode childData = childNode.object("data");
            if(!childData) {
                LogWarning("[EntityManager::setupEntityChildren] Can't find required 'data' node for internal entity in entity '%s'",
                    entity->ET_getName());
                continue;
            }
            childEntity = createEntityImpl(childData, childEntName.c_str(), finishLoad);
        } else {
            childEntity = createEntity(childEntName.c_str(), finishLoad);
        }
        if(!childEntity) {
            LogWarning("[EntityManager::setupEntityChildren] Can't create child entity '%s' for an entity: '%s'",
                childEntName, entity->ET_getName());
            continue;
        }

        Reflect::SerializeContext serCtx;
        serCtx.entityId = childEntity->getEntityId();

        Transform localTm;
        if(!tmClassInfo->readValueFrom(serCtx, &localTm, Reflect::AllClassValuesId, tmNode)) {
            LogWarning("[EntityManager::setupEntityTranform] Can't serialize 'transform' for entity '%s'", entity->ET_getName());
            continue;
        }
        entity->addChildEntityWithId(childId, *childEntity);
        childEntity->ET_setLocalTransform(localTm);
    }
    return true;
}

JSONNode EntityManager::loadEntityRootNode(const char* entityName) const {
    if(!entityName || !entityName[0]) {
        LogWarning("[EntityManager::loadEntityRootNode] Can't load entity with empty name");
        return JSONNode();
    }
    JSONNode rootNode;
    ET_SendEventReturn(rootNode, &ETAssets::ET_loadJSONAsset, entityName);
    if(!rootNode) {
        LogWarning("[EntityManager::loadEntityRootNode] Can't load entity from: '%s'", entityName);
        return JSONNode();
    }
    return rootNode;
}

Entity* EntityManager::createEntityImpl(const JSONNode& entityNode, const char* entityName, bool finishLoad) {
    if(!entityNode) {
        return nullptr;
    }
    auto entity = registry.createEntity(entityName);
    if(!entity) {
        assert(false && "Can't create entity");
        return nullptr;
    }
    if(!setupEntityChildren(entity, entityNode, finishLoad)) {
        registry.removeEntity(entity);
        return nullptr;
    }
    if(!setupEntityLogics(entity, entityNode)) {
        registry.removeEntity(entity);
        return nullptr;
    }
    if(finishLoad) {
        entity->setLoaded();
    }
    return entity;
}

Entity* EntityManager::createEntity(const char* entityName, bool finishLoad) {
    auto startTimeP = TimePoint::GetNow();

    auto entityNode = loadEntityRootNode(entityName);
    auto entity = createEntityImpl(entityNode, entityName, finishLoad);
    if(!entity) {
        return nullptr;
    }

    auto msValue = static_cast<int>(startTimeP.getMsDeltaWithNow());
    if(msValue > ENTITY_LOADING_TIME_LOG_THRESHOLD) {
        LogDebug("[EntityManager::createEntity] Create entity: '%s' (%d ms)", entityName, msValue);
    }

    return entity;
}

JSONNode EntityManager::ET_getRegisteredLogics() const {
    JSONNode node;
    for(auto& logicNode : registeredLogics) {
        auto moduleName = logicNode.second.moduleName;
        if(!node.hasKey(moduleName)) {
            JSONNode moduleNode;
            moduleNode.write(logicNode.first.c_str());
            node.write(moduleName, moduleNode);
        } else {
            auto moduleNode = node.object(moduleName);
            moduleNode.write(logicNode.first.c_str());
        }
    }
    return node;
}

Entity* EntityManager::ET_createRawEntity(const char* entityName) {
    auto entity = registry.createEntity(entityName);
    if(!entity) {
        assert(false && "Can't create entity");
        return nullptr;
    }
    return entity;
}

EntityId EntityManager::ET_createUnfinishedEntity(const char* entityName) {
    bool finishLoad = false;
    auto entity = createEntity(entityName, finishLoad);
    if(!entity) {
        LogWarning("[EntityManager::ET_createUnfinishedEntity] Can't create entity from: '%s'", entityName);
        return InvalidEntityId;
    }
    return entity->getEntityId();
}

bool EntityManager::ET_finishEntity(EntityId targetEntId) {
    std::vector<EntityId> entityTree;
    auto entity = registry.findEntity(targetEntId);
    if(!entity) {
        assert(false && "Invalid entity to finish load");
        return false;
    }
    std::vector<Entity*> traverseOrder;
    std::vector<Entity*> queue;
    queue.push_back(entity);
    while(!queue.empty()) {
        auto currEntity = queue.back();
        traverseOrder.push_back(currEntity);
        queue.pop_back();
        for(auto childEntId : currEntity->ET_getChildren()) {
            auto childEnt = registry.findEntity(childEntId);
            if(!childEnt) {
                assert(false && "Invalid children");
                return false;
            }
            queue.push_back(childEnt);
        }
    }

    for(auto it = traverseOrder.rbegin(), end = traverseOrder.rend(); it != end; ++it) {
        (*it)->setLoaded();
    }

    return true;
}

size_t EntityManager::ET_getEntitiesCount() const {
    return registry.getEntitiesCount();
}