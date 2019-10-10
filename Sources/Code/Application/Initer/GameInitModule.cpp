#include "Initer/GameInitModule.hpp"
#include "Initer/GameIniterConfig.hpp"
#include "Initer/GameConfig.hpp"
#include "Initer/GameIniter.hpp"

GameInitModule::LogicsContainerPtrT GameInitModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            GameConfig,
            GameIniter>()
        );
    return container;
}

GameInitModule::ConfigsPtrT GameInitModule::getSystemConfigs() const {
    ConfigsPtrT configs(new SystemModuleConfig<GameIniterConfig>());
    return configs;
}
