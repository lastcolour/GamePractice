#include "Entity/EntityManager.hpp"
#include "Entity/Entity.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"

#include "Game/Logics/GameBoardElemLogic.hpp"
#include "Game/Logics/GameBoardLogic.hpp"
#include "Game/Logics/GameEndTimerLogic.hpp"
#include "Game/Logics/GameEndTimerUpdaterLogic.hpp"
#include "Game/Logics/GameBoardElemSwitcherLogic.hpp"
#include "Game/Logics/GameBoardInteractionLogic.hpp"
#include "Game/Logics/GameBoardElemDestroyLogic.hpp"
#include "Game/Logics/GameScoreLogic.hpp"
#include "Game/Logics/GameScoreUpdaterLogic.hpp"

#include "Render/Logics/RenderSimpleLogic.hpp"
#include "Render/Logics/RenderTextLogic.hpp"
#include "Render/Logics/RenderImageLogic.hpp"

#include "UI/Logics/UIButton.hpp"
#include "UI/Logics/UIList.hpp"
#include "UI/Logics/UILabel.hpp"
#include "UI/Logics/UIBaseBox.hpp"
#include "UI/Logics/UIPartition.hpp"
#include "UI/Logics/UIImage.hpp"

#include <algorithm>

namespace {

const char* GAME_ENTITIES = "Game/Entities";

} // namespace

EntityManager::EntityManager() {
}

EntityManager::~EntityManager() {
    auto tempEntities = std::move(entities);
    tempEntities.clear();
}

bool EntityManager::init() {
    registerLogic<RenderSimpleLogic>("RenderSimple");
    registerLogic<RenderTextLogic>("RenderText");
    registerLogic<RenderImageLogic>("RenderImage");

    registerLogic<GameBoardLogic>("GameBoard");
    registerLogic<GameBoardElemLogic>("GameBoardElem");
    registerLogic<GameEndTimerLogic>("GameEndTimer");
    registerLogic<GameEndTimerUpdaterLogic>("GameEndTimerUpdater");
    registerLogic<GameBoardElemSwitcherLogic>("GameBoardElemSwitcher");
    registerLogic<GameBoardInteractionLogic>("GameBoardInteraction");
    registerLogic<GameBoardElemDestroyLogic>("GameBoardElemDestroy");
    registerLogic<GameScoreLogic>("GameScore");
    registerLogic<GameScoreUpdaterLogic>("GameScoreUpdater");

    registerLogic<UIBox>("UIBox");
    registerLogic<UIList>("UIList");
    registerLogic<UIButton>("UIButton");
    registerLogic<UILabel>("UILabel");
    registerLogic<UIBaseBox>("UIBaseBox");
    registerLogic<UIPartition>("UIPartition");
    registerLogic<UIImage>("UIImage");

    ETNode<ETEntityManager>::connect(getEntityId());
    return true;
}

void EntityManager::deinit() {
    ETNode<ETEntityManager>::disconnect();
}

EntityId EntityManager::ET_createEntity(const char* entityName) {
    auto obj = createEntity(nullptr, entityName);
    if(!obj) {
        return InvalidEntityId;
    }
    auto objId = obj->getEntityId();
    entities.emplace_back(std::move(obj));
    return objId;
}

void EntityManager::ET_destroyEntity(EntityId entId) {
    EntityPtrT entToRemove;
    for(auto it = entities.begin(), end = entities.end(); it != end; ++it) {
        if(*it && (*it)->getEntityId() == entId) {
            entToRemove = std::move(*it);
            break;
        }
    }
    if(entToRemove) {
        LogDebug("[EntityManager::ET_destroyEntity] Detroy entity: %s", entToRemove->ET_getName());
        entToRemove.reset();
        entities.erase(std::remove_if(entities.begin(), entities.end(), [](const EntityPtrT& obj){
            return obj == nullptr;
        }), entities.end());
    }
}

void EntityManager::registerCreateLogic(const char* logicName, LogicCreateFunc createFunc) {
    std::string reqLogicName = logicName;
    std::transform(reqLogicName.begin(), reqLogicName.end(), reqLogicName.begin(), tolower);
    auto it = logics.find(reqLogicName);
    if(it != logics.end()) {
        LogError("[EntityManager::registerCreateLogic] Register duplicate logic: '%s'", logicName);
        return;
    } else {
        logics[reqLogicName] = createFunc;
    }
}

EntityManager::LogicPtrT EntityManager::createLogic(const char* logicName) {
    std::string reqLogicName = logicName;
    std::transform(reqLogicName.begin(), reqLogicName.end(), reqLogicName.begin(), tolower);
    auto it = logics.find(reqLogicName);
    if(it != logics.end()) {
        return it->second();
    }
    return nullptr;
}

EntityManager::EntityPtrT EntityManager::createEntity(Entity* rootEnt, const char* entityName) {
    if(!entityName || !entityName[0]) {
        LogWarning("[EntityManager::createEntity] Can't create entity with empty name");
        return nullptr;
    }
    std::string entityFilePath = StringFormat("%s/%s", GAME_ENTITIES, entityName);
    JSONNode rootNode;
    ET_SendEventReturn(rootNode, &ETAssets::ET_loadJSONAsset, entityFilePath.c_str());
    if(!rootNode) {
        LogWarning("[EntityManager::createEntity] Can't load entity from: %s", entityFilePath);
        return nullptr;
    }
    EntityPtrT entPtr(new Entity(entityName, GetETSystem()->createNewEntityId()));
    auto logicsNodes = rootNode.object("logics");
    if(!logicsNodes || logicsNodes.size() == 0u) {
        LogWarning("[EntityManager::createEntity] Skip entity '%s' while it doesn't have any logic", entityName);
        return nullptr;
    }
    for(const auto& logicNode : logicsNodes) {
        std::string logicType;
        logicNode.read("type", logicType);
        auto logicPtr = createLogic(logicType.c_str());
        if(!logicPtr) {
            LogWarning("[EntityManager::createEntity] Can't find logic type '%s' for entity '%s'", logicType, entityName);
            continue;
        }
        auto logicData = logicNode.object("data");
        if(!logicData) {
            LogWarning("[EntityManager::createEntity] Can't find logic data for '%s' for entity '%s'", logicType, entityName);
            continue;
        }
        logicPtr->setEntity(entPtr.get());
        if(!logicPtr->serialize(logicData)) {
            LogWarning("[EntityManager::createEntity] Can't serialize logic '%s' for entity '%s'", logicType, entityName);
            continue;
        }
        if(!logicPtr->init()) {
            LogWarning("[EntityManager::createEntity] Can't init logic '%s' for entity '%s'", logicType, entityName);
            continue;
        }
        entPtr->addLogic(std::move(logicPtr));
    }
    auto childrenNode = rootNode.object("children");
    if (!childrenNode) {
        LogWarning("[EntityManager::createEntity] Can't find require children node in enitity file '%s'", entityName);
        return nullptr;
    }
    for(const auto& childNode : childrenNode) {
        std::string childEntName;
        childNode.read(childEntName);
        auto childEntObj = createEntity(entPtr.get(), childEntName.c_str());
        if(childEntObj) {
            entities.push_back(std::move(childEntObj));
        }
    }
    if(rootEnt) {
        rootEnt->ET_addChild(entPtr->getEntityId());
    }
    LogDebug("[EntityManager::createEntity] Create entity: '%s'", entityName);
    return std::move(entPtr);
}