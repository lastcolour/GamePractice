#include "Game/GameModule.hpp"
#include "Game/GameObjectManager.hpp"

GameModule::GameModule() :
    SystemModule("Game") {}

GameModule::LogicsContainerPtrT GameModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            GameObjectManager>()
        );
    return container;
}