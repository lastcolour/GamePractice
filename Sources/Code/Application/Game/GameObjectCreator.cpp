#include "Game/GameObjectCreator.hpp"
#include "Game/GameObject.hpp"
#include "ETApplicationInterfaces.hpp"

#include "Game/Logics/TouchLogic.hpp"
#include "Game/Logics/GameBoardLogic.hpp"
#include "Game/Logics/RenderLogic.hpp"

#include <algorithm>

namespace {
    const char* GAME_OBJECTS = "Game/GameObjects.json";
}

GameObjectCreator::GameObjectCreator() {
    registerLogic<RenderLogic>("RenderLogic");
    registerLogic<GameBoardLogic>("GameBoardLogic");
    registerLogic<TouchLogic>("TouchLogic");
}

GameObjectCreator::~GameObjectCreator() {
}

void GameObjectCreator::registerCreateLogic(const std::string& logicName, LogicCreateFunc createFunc) {
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

std::unique_ptr<BaseGameLogic> GameObjectCreator::createLogic(const std::string& logicName) {
    std::string reqLogicName = logicName;
    std::transform(reqLogicName.begin(), reqLogicName.end(), reqLogicName.begin(), tolower);
    auto it = logics.find(reqLogicName);
    if(it != logics.end()) {
        return it->second();
    }
    return nullptr;
}

std::unique_ptr<GameObject> GameObjectCreator::createObject(const std::string& objectName) {
    auto rootNode = ET_SendEventReturn(&ETAsset::ET_loadJSONAsset, GAME_OBJECTS);
    if(!rootNode) {
        LogWarning("[GameObjectCreator::createObject] Can't load game objects: %s", GAME_OBJECTS);
        return nullptr;
    }
    rootNode = rootNode.object("GameObjects");
    if(!rootNode) {
        LogWarning("[GameObjectCreator::createObject] Can't find required root node in '%s'", GAME_OBJECTS);
        return nullptr;
    }
    for(const auto& objNode : rootNode) {
        std::string baseName;
        objNode.value("name", baseName);
        if(objectName != baseName) {
            continue;
        }
        std::unique_ptr<GameObject> objPtr(new GameObject(baseName, GetEnv()->getETSystem()->createNewEntityId()));
        auto logics = objNode.object("logics");
        if(!logics || logics.size() == 0u) {
            LogWarning("[GameObjectCreator::createObject] Skip object '%s' while it doesn't have any logic", objectName);
            continue;
        }
        for(const auto& logicNode : logics) {
            std::string logicType;
            logicNode.value("type", logicType);
            auto logicPtr = createLogic(logicType);
            if(!logicPtr) {
                LogWarning("[GameObjectCreator::createObject] Can't create logic '%s' for object '%s'", logicType, objectName);
                continue;
            }
            auto logicData = logicNode.object("data");
            if(!logicData) {
                LogWarning("[GameObjectCreator::createObject] Can't find logic data for '%s' for object '%s'", logicType, objectName);
                continue;
            }
            if(!logicPtr->init(logicData)) {
                LogWarning("[GameObjectCreator::createObject] Can't init logic '%s' for object '%s'", logicType, objectName);
                continue;
            }
            logicPtr->setGameObject(objPtr.get());
            objPtr->addLogic(std::move(logicPtr));
        }
        LogDebug("[GameObjectCreator::createObject] Create object: '%s'", objectName);
        return std::move(objPtr);
    }
    return nullptr;
}