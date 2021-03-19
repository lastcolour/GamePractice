#include "Laucher/GameLaucherModule.hpp"
#include "Laucher/GameLaucher.hpp"
#include "Core/GlobalData.hpp"

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

void GameLaucherModule::createSystemConfigs() const {
}

void GameLaucherModule::registerEntityLogics(EntityLogicsRegister& logicsRegister) const {
}