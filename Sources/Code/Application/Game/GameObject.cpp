#include "Game/GameObject.hpp"
#include "Render/RenderLogic.hpp"
#include "Environment.hpp"

#include <cassert>

GameObject::GameObject(const std::string& objectName) :
    name(objectName) {

    assert(!name.empty() && "Invalid object name");
}

const std::string& GameObject::getName() const {
    return name;
}

GameObject::~GameObject() {
}

void GameObject::addRender(std::unique_ptr<RenderLogic>&& renderLogic) {
    render = std::move(renderLogic);
}

void GameObject::update() {
    if(render) {
        render->update();
    }
}

RenderLogic* GameObject::getRender() {
    return render.get();
}