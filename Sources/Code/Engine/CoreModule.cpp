#include "CoreModule.hpp"
#include "Timer.hpp"
#include "AppRunState.hpp"
#include "AssetsCacheManager.hpp"
#include "Core/ModuleConfigManager.hpp"
#include "Reflect/ClassInfoManager.hpp"

CoreModule::CoreModule() :
    SystemModule("Core") {}

CoreModule::LogicsContainerPtrT CoreModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            AppRunState,
            ClassInfoManager,
            ModuleConfigManager,
            Timer,
            AssetsCacheManager>()
        );
    return container;
}

CoreModule::ConfigsPtrT CoreModule::getSystemConfigs() const {
    return nullptr;
}