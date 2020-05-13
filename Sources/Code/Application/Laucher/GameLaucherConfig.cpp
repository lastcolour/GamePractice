#include "Laucher/GameLaucherConfig.hpp"
#include "Reflect/ReflectContext.hpp"

GameLaucherConfig::GameLaucherConfig() {
}

GameLaucherConfig::~GameLaucherConfig() {
}

void GameLaucherConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameLaucherConfig>("GameLaucherConfig")) {
        classInfo->addField("mainView", &GameLaucherConfig::mainView);
    }
}