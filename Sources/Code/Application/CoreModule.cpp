#include "CoreModule.hpp"
#include "Timer.hpp"
#include "AppRunState.hpp"
#include "AssetsCacheManager.hpp"

CoreModule::CoreModule() :
    SystemModule("Core") {}

CoreModule::LogicsContainerPtrT CoreModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            AppRunState,
            Timer,
            AssetsCacheManager>()
        );
    return container;
}