#include "Laucher/GameLaucherConfig.hpp"
#include "Core/JSONNode.hpp"

GameLaucherConfig::GameLaucherConfig() {
}

GameLaucherConfig::~GameLaucherConfig() {
}

void GameLaucherConfig::serialize(const JSONNode& node) {
    node.read("mainView", mainView);
    node.read("background", background);
}