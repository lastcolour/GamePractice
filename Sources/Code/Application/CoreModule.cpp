#include "CoreModule.hpp"
#include "Timer.hpp"

CoreModule::CoreModule() :
    SystemModule("Core") {}

CoreModule::LogicsContainerPtrT CoreModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            Timer>()
        );
    return container;
}