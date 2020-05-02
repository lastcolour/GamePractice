#include "Laucher/GameLaucherModule.hpp"
#include "Laucher/GameLaucherConfig.hpp"
#include "Laucher/GameConfig.hpp"
#include "Laucher/GameLaucher.hpp"

GameLaucherModule::LogicsContainerPtrT GameLaucherModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            GameConfig,
            GameLaucher>()
        );
    return container;
}

GameLaucherModule::ConfigsPtrT GameLaucherModule::getSystemConfigs() const {
    ConfigsPtrT configs(new SystemModuleConfig<GameLaucherConfig>());
    return configs;
}

void GameLaucherModule::registerEntityLogics(EntityLogicRegister& logicRegister) const {
}