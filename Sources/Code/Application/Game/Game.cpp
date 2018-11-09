#include "Game/Game.hpp"
#include "Game/GameObject.hpp"

Game::Game() {
}

Game::~Game() {
}

bool Game::init() {
    for(int i=0; i<10; ++i) {
        for(int j=0; j<10; ++j) {
            gameObjects.emplace_back(new GameObject(i, j));
        }
    }
    return true;
}

bool Game::shouldRun() {
    return true;
}

void Game::update() {
    for(auto& obj : gameObjects) {
        obj->update();
    }
}