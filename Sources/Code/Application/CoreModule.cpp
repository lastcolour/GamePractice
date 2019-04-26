#include "CoreModule.hpp"
#include "Core/ETSystem.hpp"
#include "Timer.hpp"

CoreModule::CoreModule() :
    SystemModule("Core") {}

CoreModule::LogicsContainerPtrT CoreModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            ETSystem,
            Timer>()
        );
    return container;
}