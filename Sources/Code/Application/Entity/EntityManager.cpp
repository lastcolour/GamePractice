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
#include "Game/Logics/GameBoardAppearAnimationLogic.hpp"
#include "Game/Logics/GameBoardMatchLogic.hpp"

#include "Render/Logics/RenderSimpleLogic.hpp"
#include "Render/Logics/RenderTextLogic.hpp"
#include "Render/Logics/RenderImageLogic.hpp"
#include "Render/Logics/RenderColoredTextureLogic.hpp"

#include "UI/Logics/UIButton.hpp"
#include "UI/Logics/UIList.hpp"
#include "UI/Logics/UILabel.hpp"
#include "UI/Logics/UIBaseBox.hpp"
#include "UI/Logics/UIPartition.hpp"
#include "UI/Logics/UIImage.hpp"
#include "UI/Logics/UIButtonPressAnimation.hpp"
#include "UI/Logics/UILabelSetter.hpp"
#include "UI/Logics/UIResultInfoSelector.hpp"
#include "UI/Logics/UISwitchControl.hpp"
#include "UI/Logics/UIAppearOnTop.hpp"
#include "UI/Logics/UIAppearFromSide.hpp"

#include "Audio/Logics/SoundPlayLogic.hpp"

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
    registerLogic<RenderColoredTextureLogic>("RenderColoredTexture");

    registerLogic<GameBoardLogic>("GameBoard");
    registerLogic<GameBoardElemLogic>("GameBoardElem");
    registerLogic<GameEndTimerLogic>("GameEndTimer");
    registerLogic<GameEndTimerUpdaterLogic>("GameEndTimerUpdater");
    registerLogic<GameBoardElemSwitcherLogic>("GameBoardElemSwitcher");
    registerLogic<GameBoardInteractionLogic>("GameBoardInteraction");
    registerLogic<GameBoardElemDestroyLogic>("GameBoardElemDestroy");
    registerLogic<GameScoreLogic>("GameScore");
    registerLogic<GameScoreUpdaterLogic>("GameScoreUpdater");
    registerLogic<GameBoardAppearAnimationLogic>("GameBoardAppearAnimation");
    registerLogic<GameBoardMatchLogic>("GameBoardMatcher");

    registerLogic<UIBox>("UIBox");
    registerLogic<UIList>("UIList");
    registerLogic<UIButton>("UIButton");
    registerLogic<UILabel>("UILabel");
    registerLogic<UIBaseBox>("UIBaseBox");
    registerLogic<UIPartition>("UIPartition");
    registerLogic<UIImage>("UIImage");
    registerLogic<UIButtonPressAnimation>("UIButtonPressAnimation");
    registerLogic<UILabelSetter>("UILabelSetter");
    registerLogic<UIResultInfoSelector>("UIResultInfoSelector");
    registerLogic<UISwitchControl>("UISwitchControl");
    registerLogic<UIAppearOnTop>("UIAppearOnTop");
    registerLogic<UIAppearFromSide>("UIAppearFromSide");

    registerLogic<SoundPlayLogic>("SoundPlayer");

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

bool EntityManager::ET_extendEntity(EntityId entityId, const char* extendEntityName) {
    auto it = std::find_if(entities.begin(), entities.end(), [entityId](const EntityPtrT& entPtr){
        return entPtr && entPtr->getEntityId() == entityId;
    });
    if(it == entities.end()) {
        LogError("[EntityManager::ET_extendEntity] Can't find entity to extend with id: %d", entityId.getRawId());
        return false;
    }
    Entity* baseEntity = it->get();
    if(!extendEntityName || !extendEntityName[0]) {
        LogError("[EntityManager::ET_extendEntity] Can't extend entity '%s' by invalid entity");
        return false;
    }
    std::string baseEntityName = baseEntity->ET_getName();
    if(baseEntityName == extendEntityName) {
        LogError("[EntityManager::ET_extendEntity] Can't extend entity '%s' by itself");
        return false;
    }

    auto node = loadEntityRootNode(extendEntityName);
    if(!node) {
        return false;
    }
    if(!setupEntityLogics(baseEntity, node, extendEntityName)) {
        return false;
    }
    if(!setupEntityChildren(baseEntity, node, extendEntityName)) {
        return false;
    }
    LogDebug("[EntityManager::ET_extendEntity] Extend entity: '%s' by '%s'", baseEntity->ET_getName(), extendEntityName);
    return true;
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

void EntityManager::registerCreateLogic(const char* logicName, LogicCreateFunc createFunc) {
    std::string reqLogicName = logicName;
    auto it = logics.find(reqLogicName);
    if(it != logics.end()) {
        LogError("[EntityManager::registerCreateLogic] Register duplicate logic: '%s'", logicName);
        return;
    } else {
        logics[reqLogicName] = createFunc;
    }
}

EntityManager::LogicPtrT EntityManager::createLogic(const char* logicName) const {
    std::string reqLogicName = logicName;
    auto it = logics.find(reqLogicName);
    if(it != logics.end()) {
        return it->second();
    }
    return nullptr;
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
        auto logicPtr = createLogic(logicType.c_str());
        if(!logicPtr) {
            LogWarning("[EntityManager::setupEntityLogics] Can't find logic type '%s' for entity '%s'", logicType, entityName);
            continue;
        }
        auto logicData = logicNode.object("data");
        if(!logicData) {
            LogWarning("[EntityManager::setupEntityLogics] Can't find logic data for '%s' for entity '%s'", logicType, entityName);
            continue;
        }
        logicPtr->setEntity(entity);
        if(!logicPtr->serialize(logicData)) {
            LogWarning("[EntityManager::setupEntityLogics] Can't serialize logic '%s' for entity '%s'", logicType, entityName);
            continue;
        }
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