#include "Platform/PlatformModule.hpp"
#include "Desktop/DesktopModule.hpp"
#include "Desktop/DesktopAssets.hpp"
#include "Desktop/GLFWSurface.hpp"

PlatformModule::PlatformModule() :
    SystemModule("DesktopModule") {}

PlatformModule::LogicsContainerPtrT PlatformModule::createSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            DesktopAssets,
            GLFWSurface>()
        );
    return container;
}

void PlatformModule::reflectSystemConfigs(ReflectContext& ctx) const {
}

void PlatformModule::registerEntityLogics(EntityLogicsRegister& logicsRegister) const {
}
