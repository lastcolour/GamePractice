#include "CoreModule.hpp"
#include "Timer.hpp"
#include "AppRunState.hpp"

CoreModule::CoreModule() :
    SystemModule("Core") {}

CoreModule::LogicsContainerPtrT CoreModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            AppRunState,
            Timer>()
        );
    return container;
}