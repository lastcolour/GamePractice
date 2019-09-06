#include "Initer/GameInitModule.hpp"
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