#include "Platforms/Desktop/DesktopModule.hpp"
#include "Platforms/Desktop/DesktopLogger.hpp"
#include "Platforms/Desktop/DesktopAssets.hpp"
#include "Platforms/Desktop/GLFWSurface.hpp"

DesktopModule::DesktopModule() :
    SystemModule("DesktopModule") {}

DesktopModule::LogicsContainerPtrT DesktopModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            DesktopLogger,
            DesktopAssets,
            GLFWSurface>()
        );
    return container;
}

DesktopModule::ConfigsPtrT DesktopModule::getSystemConfigs() const {
    return nullptr;
}
