#include "Platforms/Android/AndroidModule.hpp"
#include "Platforms/Android/AndroidSurface.hpp"
#include "Platforms/Android/AndroidEventManager.hpp"
#include "Platforms/Android/AndroidAssets.hpp"
#include "Platforms/Android/AndroidFullscreenSetup.hpp"
#include "Platforms/Android/AndroidAudioDevice.hpp"

AndroidModule::AndroidModule() :
    SystemModule("AndroidModule") {
}

AndroidModule::~AndroidModule() {
}

AndroidModule::LogicsContainerPtrT AndroidModule::createSystemLogics() const {
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

void AndroidModule::reflectSystemConfigs(ReflectContext& ctx) const {
}

void AndroidModule::registerEntityLogics(EntityLogicsRegister& logicsRegister) const {
}