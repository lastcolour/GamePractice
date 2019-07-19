#include "Game/GameObjectManager.hpp"
#include "Game/GameObject.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"

#include "Game/Logics/GameBoardElemLogic.hpp"
#include "Game/Logics/GameBoardLogic.hpp"
#include "Render/Logics/RenderSimpleLogic.hpp"
#include "Render/Logics/RenderTextLogic.hpp"

#include "UI/Logics/UIButton.hpp"
#include "UI/Logics/UIList.hpp"
#include "UI/Logics/UILabel.hpp"

#include <algorithm>

namespace {
    const char* GAME_OBJECTS = "Game/GameObjects";
} // namespace

GameObjectManager::GameObjectManager() {
}

GameObjectManager::~GameObjectManager() {
}

bool GameObjectManager::init() {
    registerLogic<RenderSimpleLogic>("RenderSimple");
    registerLogic<RenderTextLogic>("RenderText");

    registerLogic<GameBoardLogic>("GameBoard");
    registerLogic<GameBoardElemLogic>("GameBoardElem");

    registerLogic<UIBox>("UIBox");
    registerLogic<UIList>("UIList");
    registerLogic<UIButton>("UIButton");
    registerLogic<UILabel>("UILabel");

    ETNode<ETGameObjectManager>::connect(getEntityId());
    return true;
}

void GameObjectManager::deinit() {
    ETNode<ETGameObjectManager>::disconnect();
}

EntityId GameObjectManager::ET_createGameObject(const std::string& objectName) {
    auto obj = createObject(objectName);
    if(!obj) {
        return InvalidEntityId;
    }
    auto objId = obj->getEntityId();
    gameObjects.emplace_back(std::move(obj));
    return objId;
}

void GameObjectManager::ET_destroyObject(EntityId entId) {
    auto eraseIt = gameObjects.end();
    for(auto it = gameObjects.begin(), end = gameObjects.end(); it != end; ++it) {
        if((*it)->getEntityId() == entId) {
            eraseIt = it;
            break;
        }
    }
    if(eraseIt != gameObjects.end()) {
        gameObjects.erase(eraseIt);
    }
}

void GameObjectManager::registerCreateLogic(const std::string& logicName, LogicCreateFunc createFunc) {
    std::string reqLogicName = logicName;
    std::transform(reqLogicName.begin(), reqLogicName.end(), reqLogicName.begin(), tolower);
    auto it = logics.find(reqLogicName);
    if(it != logics.end()) {
        LogError("[GameObjectCreator::registerCreateLogic] Register duplicate logic: '%s'", logicName);
        return;
    } else {
        logics[reqLogicName] = createFunc;
    }
}

std::unique_ptr<GameLogic> GameObjectManager::createLogic(const std::string& logicName) {
    std::string reqLogicName = logicName;
    std::transform(reqLogicName.begin(), reqLogicName.end(), reqLogicName.begin(), tolower);
    auto it = logics.find(reqLogicName);
    if(it != logics.end()) {
        return it->second();
    }
    return nullptr;
}

std::unique_ptr<GameObject> GameObjectManager::createObject(const std::string& objectName) {
    std::string objectFilePath = StringFormat("%s/%s", GAME_OBJECTS, objectName);
    JSONNode rootNode;
    ET_SendEventReturn(rootNode, &ETAssets::ET_loadJSONAsset, objectFilePath);
    if(!rootNode) {
        LogWarning("[GameObjectManager::createObject] Can't load game objects from: %s", objectFilePath);
        return nullptr;
    }
    std::unique_ptr<GameObject> objPtr(new GameObject(objectName, GetETSystem()->createNewEntityId()));
    auto logicsNodes = rootNode.object("logics");
    if(!logicsNodes || logicsNodes.size() == 0u) {
        LogWarning("[GameObjectManager::createObject] Skip object '%s' while it doesn't have any logic", objectName);
        return nullptr;
    }
    for(const auto& logicNode : logicsNodes) {
        std::string logicType;
        logicNode.value("type", logicType);
        auto logicPtr = createLogic(logicType);
        if(!logicPtr) {
            LogWarning("[GameObjectManager::createObject] Can't find logic type '%s' for object '%s'", logicType, objectName);
            continue;
        }
        auto logicData = logicNode.object("data");
        if(!logicData) {
            LogWarning("[GameObjectManager::createObject] Can't find logic data for '%s' for object '%s'", logicType, objectName);
            continue;
        }
        logicPtr->setGameObject(objPtr.get());
        if(!logicPtr->serialize(logicData)) {
            LogWarning("[GameObjectManager::createObject] Can't serialize logic '%s' for object '%s'", logicType, objectName);
            continue;
        }
        if(!logicPtr->init()) {
            LogWarning("[GameObjectManager::createObject] Can't init logic '%s' for object '%s'", logicType, objectName);
            continue;
        }
        objPtr->addLogic(std::move(logicPtr));
    }
    LogDebug("[GameObjectManager::createObject] Create object: '%s'", objectName);
    return std::move(objPtr);
}