#include "Game/Game.hpp"
#include "Game/GameObject.hpp"
#include "ETApplicationInterfaces.hpp"

namespace {
    const char* GAME_ROOT_OBJECT = "MainMenu";
}

Game::Game() :
    rootObject(GAME_ROOT_OBJECT),
    gameObjects() {
}

Game::~Game() {
    ETNode<ETGame>::disconnect();
}

void Game::setRootObject(const std::string& rootObjectName) {
    rootObject = rootObjectName;
}

bool Game::init() {
    ETNode<ETGame>::connect(getEntityId());
    if(rootObject.empty()) {
        LogDebug("[Game::onInit] Skip init phase while game doesn't have root object");
        return true;
    }
    auto rootObjId = ET_createGameObject(GAME_ROOT_OBJECT);
    if(rootObjId == InvalidEntityId) {
        LogError("[Game::onInit] Can't create game root object: %s", GAME_ROOT_OBJECT);
        return false;
    }
    return true;
}

void Game::deinit() {
    ETNode<ETGame>::disconnect();
}

EntityId Game::ET_createGameObject(const std::string& objectName) {
    auto obj = gameObjCreator.createObject(objectName);
    if(!obj) {
        return InvalidEntityId;
    }
    auto objId = obj->getEntityId();
    gameObjects.emplace_back(std::move(obj));
    return objId;
}

void Game::ET_destroyObject(EntityId entId) {
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
