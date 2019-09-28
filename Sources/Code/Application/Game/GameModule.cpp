#include "Game/GameModule.hpp"
#include "Game/GameTimer.hpp"
#include "Game/GameStateManager.hpp"

GameModule::LogicsContainerPtrT GameModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            GameTimer,
            GameStateManager>()
        );
    return container;
}