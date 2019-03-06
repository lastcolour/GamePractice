#include "Game/GameLogic.hpp"

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