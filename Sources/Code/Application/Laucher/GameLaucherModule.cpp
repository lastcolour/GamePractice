#include "Laucher/GameLaucherModule.hpp"
#include "Laucher/GameLaucherConfig.hpp"
#include "Laucher/GameLaucher.hpp"
#include "Reflect/ReflectContext.hpp"

GameLaucherModule::GameLaucherModule() :
    SystemModule("GameLaucher") {}

GameLaucherModule::~GameLaucherModule() {
}

GameLaucherModule::LogicsContainerPtrT GameLaucherModule::createSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            GameLaucher>()
        );
    return container;
}

void GameLaucherModule::reflectSystemConfigs(ReflectContext& ctx) const {
    ctx.reflect<GameLaucherConfig>();
}

void GameLaucherModule::registerEntityLogics(EntityLogicsRegister& logicsRegister) const {
}