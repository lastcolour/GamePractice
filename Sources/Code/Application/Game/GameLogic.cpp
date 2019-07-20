#include "Game/GameLogic.hpp"
#include "Game/GameObject.hpp"

#include <cassert>

GameLogic::GameLogic() :
    gameObj(nullptr) {
}

GameLogic::~GameLogic() {
}

void GameLogic::setGameObject(const GameObject* obj) {
    gameObj = obj;
}

EntityId GameLogic::getEntityId() const {
    if(gameObj) {
        return gameObj->getEntityId();
    }
    assert(false && "No valid game object to query entity Id");
    return InvalidEntityId;
}

EntityId GameLogic::getParentId() const {
    if(gameObj) {
        return gameObj->ET_getParentId();
    }
    assert(false && "No valid game object to query parent's entity Id");
    return InvalidEntityId;
}

const char* GameLogic::getEntityName() const {
    if(gameObj) {
        return gameObj->ET_getName();
    }
    assert(false && "No valid game object to query entity name");
    return "";
}