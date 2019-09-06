#include "Game/GameModule.hpp"
#include "Game/GameTimer.hpp"
#include "Game/GameState.hpp"

GameModule::LogicsContainerPtrT GameModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            GameTimer,
            GameState>()
        );
    return container;
}