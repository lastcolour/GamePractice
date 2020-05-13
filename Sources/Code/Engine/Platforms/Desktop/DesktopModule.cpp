#include "Platforms/Desktop/DesktopModule.hpp"
#include "Platforms/Desktop/DesktopAssets.hpp"
#include "Platforms/Desktop/GLFWSurface.hpp"

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

void DesktopModule::reflectSystemConfigs(ReflectContext& ctx) const {
}

void DesktopModule::registerEntityLogics(EntityLogicRegister& logicRegister) const {
}
