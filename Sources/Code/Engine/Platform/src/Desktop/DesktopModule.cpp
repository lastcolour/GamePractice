#include "Desktop/DesktopModule.hpp"
#include "Desktop/DesktopAssets.hpp"
#include "Desktop/GLFWSurface.hpp"

DesktopModule::DesktopModule() :
    SystemModule("DesktopModule") {}

DesktopModule::LogicsContainerPtrT DesktopModule::createSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            DesktopAssets,
            GLFWSurface>()
        );
    return container;
}

void DesktopModule::createSystemConfigs() const {
}

void DesktopModule::registerEntityLogics(EntityLogicsRegister& logicsRegister) const {
}
