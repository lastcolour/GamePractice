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

void GameObject::addLogic(std::unique_ptr<BaseGameLogic>&& logic) {
    assert(logic && "Add invalid render logic");
    logic->setGameObject(this);
    logics.emplace_back(std::move(logic));
}

void GameObject::update() {
    for(auto& logic : logics) {
        logic->update();
    }
}