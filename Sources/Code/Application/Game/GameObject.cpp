#include "Game/GameObject.hpp"
#include "Game/GameLogic.hpp"

#include <cassert>

GameObject::GameObject(const std::string& objectName, int entId) :
    name(objectName),
    entityId(entId) {

    assert(!name.empty() && "Invalid object name");

    ETNode<ETGameObject>::connect(getEntityId());
}

GameObject::~GameObject() {
}

void GameObject::addLogic(std::unique_ptr<GameLogic>&& logic) {
    assert(logic && "Add invalid game logic");
    logics.emplace_back(std::move(logic));
}