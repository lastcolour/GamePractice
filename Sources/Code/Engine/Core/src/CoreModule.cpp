#include "CoreModule.hpp"
#include "Timer.hpp"
#include "AppRunState.hpp"
#include "AssetsCacheManager.hpp"
#include "ModuleConfigManager.hpp"
#include "Reflect/ClassInfoManager.hpp"

#ifdef PLATFORM_ANDROID
  #include "Platforms/Android/AndroidLogger.hpp"
  using PlatformLogger = AndroidLogger;
#else
  #include "Platforms/Desktop/DesktopLogger.hpp"
  using PlatformLogger = DesktopLogger;
#endif

CoreModule::CoreModule() :
    SystemModule("Core") {}

CoreModule::LogicsContainerPtrT CoreModule::createSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            AppRunState,
            ClassInfoManager,
            ModuleConfigManager,
            PlatformLogger,
            Timer,
            AssetsCacheManager>()
        );
    return container;
}

void CoreModule::reflectSystemConfigs(ReflectContext& ctx) const {
}

void CoreModule::registerEntityLogics(EntityLogicRegister& logicRegister) const {
}