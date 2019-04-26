#include "Game/GameModule.hpp"
#include "Game/Game.hpp"

GameModule::GameModule() :
    SystemModule("Game") {}

GameModule::LogicsContainerPtrT GameModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            Game>()
        );
    return container;
}