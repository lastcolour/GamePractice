#include "CoreModule.hpp"
#include "Timer.hpp"
#include "AppRunState.hpp"
#include "AssetsCacheManager.hpp"
#include "Core/ModuleConfigManager.hpp"

CoreModule::CoreModule() :
    SystemModule("Core") {}

CoreModule::LogicsContainerPtrT CoreModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            ModuleConfigManager,
            AppRunState,
            Timer,
            AssetsCacheManager>()
        );
    return container;
}

CoreModule::ConfigsPtrT CoreModule::getSystemConfigs() const {
    return nullptr;
}