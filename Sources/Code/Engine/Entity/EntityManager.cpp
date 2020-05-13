#include "Entity/EntityManager.hpp"
#include "Entity/Entity.hpp"
#include "Entity/EntityLogic.hpp"
#include "Entity/EntityLogicRegister.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"

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
    EntityPtrT entToRemove;
    for(auto it = entities.begin(), end = entities.end(); it != end; ++it) {
        if(*it && (*it)->getEntityId() == entityId) {
            entToRemove = std::move(*it);
            break;
        }
    }
    if(entToRemove) {
        LogDebug("[EntityManager::ET_destroyEntity] Detroy entity: '%s'", entToRemove->ET_getName());
        entToRemove.reset();
        entities.erase(std::remove_if(entities.begin(), entities.end(), [](const EntityPtrT& obj){
            return obj == nullptr;
        }), entities.end());
    }
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
        auto logicPtr = logicInstance.acquire<EntityLogic>();
        if(!logicPtr) {
            LogWarning("[EntityManager::setupEntityLogics] Can't acquire logic of type '%s' for entity '%s'", logicType, entityName);
            return false;
        }
        logicPtr->setEntity(entity);
        if(!logicPtr->init()) {
            LogWarning("[EntityManager::setupEntityLogics] Can't init logic '%s' for entity '%s'", logicType, entityName);
            continue;
        }
        entity->addLogic(std::move(logicPtr));
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

Entity* EntityManager::createEntityImpl(Entity* rootEntity, const char* entityName) {
    auto rootNode = loadEntityRootNode(entityName);
    if(!rootNode) {
        return nullptr;
    }
    entities.emplace_back(new Entity(entityName, GetETSystem()->createNewEntityId()));
    Entity* entity = entities.back().get();
    if(!setupEntityLogics(entity, rootNode, entityName)) {
        entities.pop_back();
        return nullptr;
    }
    if(!setupEntityChildren(entity, rootNode, entityName)) {
        entities.pop_back();
        return nullptr;
    }
    if(rootEntity) {
        rootEntity->ET_addChild(entity->getEntityId());
    }
    return entity;
}

Entity* EntityManager::createEntity(Entity* rootEntity, const char* entityName) {
    auto startTimeP = std::chrono::high_resolution_clock::now();

    auto entity = createEntityImpl(rootEntity, entityName);

    auto duration =  std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - startTimeP).count();

    LogDebug("[EntityManager::createEntity] Create entity: '%s' (%d ms)", entityName, duration);

    return entity;
}