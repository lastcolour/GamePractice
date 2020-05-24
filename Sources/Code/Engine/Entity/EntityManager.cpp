#include "Entity/EntityManager.hpp"
#include "Entity/Entity.hpp"
#include "Entity/EntityLogic.hpp"
#include "Entity/EntityLogicRegister.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"
#include "Core/MemoryStream.hpp"

#include <algorithm>
#include <cassert>

namespace {

const char* GAME_ENTITIES = "Entities";

} // namespace

EntityManager::EntityManager() {
}

EntityManager::~EntityManager() {
    auto tempEntities = std::move(entities);
    tempEntities.clear();
}

bool EntityManager::init() {
    ETNode<ETEntityManager>::connect(getEntityId());
    return true;
}

void EntityManager::deinit() {
    ETNode<ETEntityManager>::disconnect();
}

EntityId EntityManager::ET_createEntity(const char* entityName) {
    auto entity = createEntity(nullptr, entityName);
    if(entity) {
        return entity->getEntityId();
    }
    return InvalidEntityId;
}

void EntityManager::ET_destroyEntity(EntityId entityId) {
    auto it = entities.find(entityId);
    if(it == entities.end()) {
        LogWarning("[EntityManager::ET_destroyEntity] Can't find entity to destroy");
        return;
    }
    LogDebug("[EntityManager::ET_destroyEntity] Destroy entity: '%s'", it->second->ET_getName());
    entities.erase(it);
}

bool EntityManager::ET_registerLogics(EntityLogicRegister& logicRegister) {
    for(auto classInfo : logicRegister.getLogicClasses()) {
        assert(classInfo && "Invalid logic class info");
        auto it = registeredLogics.find(classInfo->getName());
        if(it != registeredLogics.end()) {
            LogError("[EntityManager::ET_registerLogics] Logic already registered with such name: '%s'", classInfo->getName());
            return false;
        }
        registeredLogics[classInfo->getName()] = classInfo;
    }
    return true;
}

EntityId EntityManager::ET_createEntityFromJSON(const JSONNode& node, const char* entityName) {
    if(!entityName || !entityName[0]) {
        LogWarning("[EntityManager::ET_createEntityFromJSON] Can't create entity with empty name");
        return InvalidEntityId;
    }
    if(!node) {
        LogWarning("[EntityManager::ET_createEntityFromJSON] Invalid JSON node to create entity '%s' from, entityName");
        return InvalidEntityId;
    }

    auto startTimeP = std::chrono::high_resolution_clock::now();

    auto entity = createEntityImpl(nullptr, node, entityName);
    if(!entity) {
        return InvalidEntityId;
    }

    auto duration =  std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - startTimeP).count();

    LogDebug("[EntityManager::createEntity] Create entity: '%s' (%d ms)", entityName, duration);

    return entity->getEntityId();
}

EntityLogicId EntityManager::ET_addLogicToEntity(EntityId entityId, const char* logicName) {
    if(!entityId.isValid()) {
        LogError("[EntityManager::ET_addLogicToEntity] Invalid entity id to add logic");
        return InvalidEntityLogicId;
    }
    auto entIt = entities.find(entityId);
    if(entIt == entities.end()) {
        LogWarning("[EntityManager::ET_addLogicToEntity] Can't find entity to add a logic '%s'", logicName);
        return InvalidEntityLogicId;
    }
    if(!logicName || !logicName[0]) {
        LogWarning("[EntityManager::ET_addLogicToEntity] Can't add logic with empty name");
        return InvalidEntityLogicId;
    }
    Entity* entity = entIt->second.get();
    auto logicIt = registeredLogics.find(logicName);
    if(logicIt == registeredLogics.end()) {
        LogWarning("[EntityManager::ET_addLogicToEntity] Can't find logic '%s' to add to an entity '%s'",
            logicName, entIt->second->ET_getName());
        return InvalidEntityLogicId;
    }
    auto logicInstance = logicIt->second->createDefaultInstance();
    if(!logicInstance.get()) {
        LogWarning("[EntityManager::ET_addLogicToEntity] Can't create instance of logic '%s' to add to an entity '%s'",
            logicName, entIt->second->ET_getName());
        return InvalidEntityLogicId;
    }
    auto logicId = entity->addLogic(std::move(logicInstance));
    if(logicId == InvalidEntityLogicId) {
        LogWarning("[EntityManager::setupEntityLogics] Can't init logic '%s' for entity '%s'", logicName, entity->ET_getName());
        return InvalidEntityLogicId;
    }
    LogDebug("[EntityManager::ET_addLogicToEntity] Add logic '%s' to entity '%s'", logicName, entity->ET_getName());
    return logicId;
}

void EntityManager::ET_removeLogicFromEntity(EntityId entityId, EntityLogicId logicId) {
    if(!entityId.isValid()) {
        LogError("[EntityManager::ET_removeLogicFromEntity] Invalid entity id to remove logic");
        return;
    }
    auto entId = entities.find(entityId);
    if(entId == entities.end()) {
        LogWarning("[EntityManager::ET_removeLogicFromEntity] Can't find entity to remove logic with id '%d'",
            logicId);
        return;
    }
    Entity* entity = entId->second.get();
    if(logicId == InvalidEntityLogicId) {
        LogWarning("[EntityManager::ET_removeLogicFromEntity] Can't remove logic with invalid id from entity '%s'", entity->ET_getName());
        return;
    }
    if(!entity->removeLogic(logicId)) {
        LogWarning("[EntityManager::ET_removeLogicFromEntity] Can't find logic with id '%d' to remove for entity '%s'",
            logicId, entity->ET_getName());
        return;
    }
    LogDebug("[EntityManager::ET_removeLogicFromEntity] Remove logic with id '%d' from entity '%s'",
        logicId, entity->ET_getName());
}

bool EntityManager::ET_readEntityLogicData(EntityId entityId, EntityLogicId logicId, MemoryStream& stream) {
    if(entityId == InvalidEntityId) {
        LogWarning("[EntityManager::ET_readEntityLogicData] Can't read entity logic data from entity with invalid id");
        return false;
    }
    auto it = entities.find(entityId);
    if(it == entities.end()) {
        LogWarning("[EntityManager::ET_readEntityLogicData] Can't find entity to read logic data");
        return false;
    }
    auto entity = it->second.get();
    if(logicId == InvalidEntityLogicId) {
        LogWarning("[EntityManager::ET_readEntityLogicData] Can't read logic data with invalid logic id from entity: '%s'",
            entity->ET_getName());
        return false;
    }
    if(!entity->readLogicData(logicId, stream)) {
        LogWarning("[EntityManager::ET_readEntityLogicData] Error to read logic data from entity: '%s'",
            entity->ET_getName());
        return false;
    }
    return true;
}

bool EntityManager::ET_readEntityLogicValueData(EntityId entityId, EntityLogicId logicId,
    EntityLogicValueId valueId, MemoryStream& stream) {
    if(entityId == InvalidEntityId) {
        LogWarning("[EntityManager::ET_readEntityLogicValueData] Can't read entity logic value data from entity with invalid id");
        return false;
    }
    auto it = entities.find(entityId);
    if(it == entities.end()) {
        LogWarning("[EntityManager::ET_readEntityLogicValueData] Can't find entity to read logic value data");
        return false;
    }
    auto entity = it->second.get();
    if(logicId == InvalidEntityLogicId) {
        LogWarning("[EntityManager::ET_readEntityLogicValueData] Can't read logic value data with invalid logic id from entity: '%s'",
            entity->ET_getName());
        return false;
    }
    if(valueId == InvalidEntityLogicValueId) {
        LogWarning("[EntityManager::ET_readEntityLogicValueData] Can't read logic value data with invalid value id from entity: '%s'",
            entity->ET_getName());
        return false;
    }
    if(!entity->readLogicValueData(logicId, valueId, stream)) {
        LogWarning("[EntityManager::ET_readEntityLogicValueData] Error to read logic value data from entity: '%s'",
            entity->ET_getName());
        return false;
    }
    return true;
}

bool EntityManager::ET_writeEntityLogicData(EntityId entityId, EntityLogicId logicId, MemoryStream& stream) {
    if(entityId == InvalidEntityId) {
        LogWarning("[EntityManager::ET_writeEntityLogicData] Can't write entity logic data from entity with invalid id");
        return false;
    }
    auto it = entities.find(entityId);
    if(it == entities.end()) {
        LogWarning("[EntityManager::ET_writeEntityLogicData] Can't find entity to write logic data");
        return false;
    }
    auto entity = it->second.get();
    if(logicId == InvalidEntityLogicId) {
        LogWarning("[EntityManager::ET_writeEntityLogicData] Can't write logic data with invalid logic id from entity: '%s'",
            entity->ET_getName());
        return false;
    }
    if(!entity->writeLogicData(logicId, stream)) {
        LogWarning("[EntityManager::ET_writeEntityLogicData] Error to write logic data to entity: '%s'",
            entity->ET_getName());
        return false;
    }
    return true;
}

bool EntityManager::ET_writeEntityLogicValueData(EntityId entityId, EntityLogicId logicId,
    EntityLogicValueId valueId, MemoryStream& stream) {
    if(entityId == InvalidEntityId) {
        LogWarning("[EntityManager::ET_writeEntityLogicValueData] Can't write entity logic value data from entity with invalid id");
        return false;
    }
    auto it = entities.find(entityId);
    if(it == entities.end()) {
        LogWarning("[EntityManager::ET_writeEntityLogicValueData] Can't write entity to read logic value data");
        return false;
    }
    auto entity = it->second.get();
    if(logicId == InvalidEntityLogicId) {
        LogWarning("[EntityManager::ET_writeEntityLogicValueData] Can't write logic value data with invalid logic id from entity: '%s'",
            entity->ET_getName());
        return false;
    }
    if(valueId == InvalidEntityLogicValueId) {
        LogWarning("[EntityManager::ET_writeEntityLogicValueData] Can't write logic value data with invalid value id from entity: '%s'",
            entity->ET_getName());
        return false;
    }
    if(!entity->writeLogicValueData(logicId, valueId, stream)) {
        LogWarning("[EntityManager::ET_writeEntityLogicValueData] Error to write logic value data to entity: '%s'",
            entity->ET_getName());
        return false;
    }
    return true;
}

bool EntityManager::setupEntityLogics(Entity* entity, const JSONNode& node, const char* entityName) const {
    auto logicsNodes = node.object("logics");
    if(!logicsNodes || logicsNodes.size() == 0u) {
        LogWarning("[EntityManager::setupEntityLogics] Skip entity '%s' while it doesn't have any logic", entityName);
        return false;
    }
    for(const auto& logicNode : logicsNodes) {
        std::string logicType;
        logicNode.read("type", logicType);
        auto it = registeredLogics.find(logicType);
        if(it == registeredLogics.end()) {
            LogWarning("[EntityManager::setupEntityLogics] Can't find logic type '%s' for entity '%s'", logicType, entityName);
            continue;
        }
        auto& logicClassInfo = *(it->second);
        auto logicData = logicNode.object("data");
        if(!logicData) {
            LogWarning("[EntityManager::setupEntityLogics] Can't find logic data for '%s' for entity '%s'", logicType, entityName);
            return false;
        }
        auto logicInstance = logicClassInfo.createInstance(logicData);
        if(!logicInstance.get()) {
            LogWarning("[EntityManager::setupEntityLogics] Can't create instance of logic type '%s' for entity '%s'", logicType, entityName);
            return false;
        }
        auto logicId = entity->addLogic(std::move(logicInstance));
        if(logicId == InvalidEntityLogicId) {
            LogWarning("[EntityManager::setupEntityLogics] Can't init logic '%s' for entity '%s'", logicType, entityName);
            continue;
        }
    }
    return true;
}

bool EntityManager::setupEntityChildren(Entity* entity, const JSONNode& node, const char* entityName) {
    auto childrenNode = node.object("children");
    if(!childrenNode) {
        LogWarning("[EntityManager::setupEntityChildren] Can't find require children node in enitity file '%s'", entityName);
        return false;
    }
    for(const auto& childNode : childrenNode) {
        std::string childEntName;
        childNode.read(childEntName);
        createEntity(entity, childEntName.c_str());
    }
    return true;
}

JSONNode EntityManager::loadEntityRootNode(const char* entityName) const {
    if(!entityName || !entityName[0]) {
        LogWarning("[EntityManager::loadEntityRootNode] Can't load entity with empty name");
        return JSONNode();
    }
    std::string entityFilePath = StringFormat("%s/%s", GAME_ENTITIES, entityName);
    JSONNode rootNode;
    ET_SendEventReturn(rootNode, &ETAssets::ET_loadJSONAsset, entityFilePath.c_str());
    if(!rootNode) {
        LogWarning("[EntityManager::loadEntityRootNode] Can't load entity from: '%s'", entityFilePath);
        return JSONNode();
    }
    return rootNode;
}

Entity* EntityManager::createEntityImpl(Entity* rootEntity, const JSONNode& entityNode, const char* entityName) {
    if(!entityNode) {
        return nullptr;
    }
    EntityPtrT entity(new Entity(entityName, GetETSystem()->createNewEntityId()));
    if(!setupEntityLogics(entity.get(), entityNode, entityName)) {
        return nullptr;
    }
    if(!setupEntityChildren(entity.get(), entityNode, entityName)) {
        return nullptr;
    }
    auto resEntPtr = entity.get();
    entities[resEntPtr->getEntityId()] = std::move(entity);
    if(rootEntity) {
        rootEntity->ET_addChild(entity->getEntityId());
    }
    return resEntPtr;
}

Entity* EntityManager::createEntity(Entity* rootEntity, const char* entityName) {
    auto startTimeP = std::chrono::high_resolution_clock::now();

    auto entityNode = loadEntityRootNode(entityName);
    auto entity = createEntityImpl(rootEntity, entityNode, entityName);

    auto duration =  std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - startTimeP).count();

    LogDebug("[EntityManager::createEntity] Create entity: '%s' (%d ms)", entityName, duration);

    return entity;
}

void EntityManager::ET_getRegisteredLogics(std::vector<const char*>& logicNames) {
    logicNames.reserve(registeredLogics.size());
    for(const auto& logicClassInfo : registeredLogics) {
        logicNames.push_back(logicClassInfo.second->getName());
    }
}