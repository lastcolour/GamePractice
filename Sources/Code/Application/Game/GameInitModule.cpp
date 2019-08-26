#include "GameInitModule.hpp"
#include "Game/GameConfig.hpp"
#include "Game/GameIniter.hpp"

GameInitModule::LogicsContainerPtrT GameInitModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            GameConfig,
            GameIniter>()
        );
    return container;
}