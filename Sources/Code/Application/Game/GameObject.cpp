#include "Game/GameObject.hpp"
#include "Environment.hpp"
#include "Platforms/Surface.hpp"
#include "Platforms/OpenGL.hpp"

GameObject::GameObject(int i, int j) {
}

GameObject::~GameObject() {
}

void GameObject::update() {
    auto surf = GetEnv()->getSurface();
    auto w = surf->getWidth();
    auto h = surf->getHeight();
}