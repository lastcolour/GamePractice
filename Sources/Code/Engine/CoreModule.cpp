#include "CoreModule.hpp"
#include "Timer.hpp"
#include "AppRunState.hpp"
#include "AssetsCacheManager.hpp"
#include "Core/ModuleConfigManager.hpp"
#include "Reflect/ClassInfoManager.hpp"

CoreModule::CoreModule() :
    SystemModule("Core") {}

CoreModule::LogicsContainerPtrT CoreModule::createSystemLogics() const {
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

void CoreModule::reflectSystemConfigs(ReflectContext& ctx) const {
}

void CoreModule::registerEntityLogics(EntityLogicRegister& logicRegister) const {
}