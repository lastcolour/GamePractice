#include "Laucher/GameLaucherConfig.hpp"

GameLaucherConfig::GameLaucherConfig() {
}

GameLaucherConfig::~GameLaucherConfig() {
}

void GameLaucherConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameLaucherConfig>("GameLaucherConfig")) {
    }
}