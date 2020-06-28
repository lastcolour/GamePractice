#include "Core/CoreModule.hpp"
#include "MainTimer.hpp"
#include "AppRunState.hpp"
#include "AssetsCacheManager.hpp"
#include "ModuleSharedManager.hpp"
#include "Reflect/ClassInfoManager.hpp"
#include "Platform/PlatformLogger.hpp"

CoreModule::CoreModule() :
    SystemModule("Core") {}

CoreModule::LogicsContainerPtrT CoreModule::createSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            AppRunState,
            ClassInfoManager,
            ModuleSharedManager,
            PlatformLogger,
            MainTimer,
            AssetsCacheManager>()
        );
    return container;
}

void CoreModule::reflectSystemConfigs(ReflectContext& ctx) const {
}

void CoreModule::registerEntityLogics(EntityLogicsRegister& logicsRegister) const {
}