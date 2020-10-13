#include "CoreModule.hpp"
#include "Core/App/AppRunState.hpp"
#include "Core/App/AssetsCacheManager.hpp"
#include "Core/App/ModuleSharedManager.hpp"
#include "Core/App/AsyncAssetsLoader.hpp"
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
            AsyncAssetsLoader,
            AssetsCacheManager>()
        );
    return container;
}

void CoreModule::reflectSystemConfigs(ReflectContext& ctx) const {
}

void CoreModule::registerEntityLogics(EntityLogicsRegister& logicsRegister) const {
}