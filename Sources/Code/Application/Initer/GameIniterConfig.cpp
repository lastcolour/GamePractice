#include "Initer/GameIniterConfig.hpp"
#include "Core/JSONNode.hpp"

GameIniterConfig::GameIniterConfig() {
}

GameIniterConfig::~GameIniterConfig() {
}

void GameIniterConfig::serialize(const JSONNode& node) {
    node.read("mainView", mainView);
    node.read("background", background);
}