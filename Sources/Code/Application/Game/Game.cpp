#include "Game/Game.hpp"
#include "Game/GameObject.hpp"
#include "ETApplicationInterfaces.hpp"

namespace {
    const char* GAME_ROOT_OBJECT = "GameRootObject";
}

Game::Game() :
    gameObjects() {
}

Game::~Game() {
}

bool Game::onInit() {
    ETNode<ETGame>::connect(getEntityId());

    auto rootObjId = ET_createGameObject(GAME_ROOT_OBJECT);
    if(rootObjId == InvalidEntityId) {
        LogError("[Game::init] Can't create game root object: %s", GAME_ROOT_OBJECT);
        return false;
    }
    return true;
}

bool Game::onShouldRun() {
    return true;
}

void Game::onUpdate() {
    for(auto& obj : gameObjects) {
        obj->update();
    }
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
