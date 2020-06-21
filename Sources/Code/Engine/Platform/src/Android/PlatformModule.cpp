#include "Platform/PlatformModule.hpp"

#include "Android/AndroidSurface.hpp"
#include "Android/AndroidEventManager.hpp"
#include "Android/AndroidAssets.hpp"
#include "Android/AndroidFullscreenSetup.hpp"
#include "Android/AndroidAudioDevice.hpp"

PlatformModule::PlatformModule() :
    SystemModule("PlatformModule") {
}

PlatformModule::~PlatformModule() {
}

PlatformModule::LogicsContainerPtrT PlatformModule::createSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            AndroidAssets,
            AndroidEventManager,
            AndroidSurface,
            AndroidFullscreenSetup,
            AndroidAudioDevice>()
        );
    return container;
}

void PlatformModule::reflectSystemConfigs(ReflectContext& ctx) const {
}

void PlatformModule::registerEntityLogics(EntityLogicsRegister& logicsRegister) const {
}