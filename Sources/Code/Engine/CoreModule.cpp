#include "CoreModule.hpp"
#include "Core/App/AppRunState.hpp"
#include "Core/App/AssetsCacheManager.hpp"
#include "Core/App/AsyncAssetsLoader.hpp"
#include "Platform/PlatformLogger.hpp"

CoreModule::CoreModule() :
    SystemModule("Core") {}

CoreModule::LogicsContainerPtrT CoreModule::createSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            AppRunState,
            PlatformLogger,
            AsyncAssetsLoader,
            AssetsCacheManager>()
        );
    return container;
}

void CoreModule::createSystemConfigs() const {
}

void CoreModule::registerEntityLogics(EntityLogicsRegister& logicsRegister) const {
}