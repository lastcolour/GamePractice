#include "Platforms/Android/AndroidModule.hpp"
#include "Platforms/Android/AndroidSurface.hpp"
#include "Platforms/Android/AndroidEventManager.hpp"
#include "Platforms/Android/AndroidLogger.hpp"
#include "Platforms/Android/AndroidAssets.hpp"
#include "Platforms/Android/AndroidFullscreenSetup.hpp"
#include "Platforms/Android/AndroidAudioDevice.hpp"

AndroidModule::AndroidModule() :
    SystemModule("AndroidModule") {
}

AndroidModule::~AndroidModule() {
}

AndroidModule::LogicsContainerPtrT AndroidModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            AndroidLogger,
            AndroidAssets,
            AndroidEventManager,
            AndroidSurface,
            AndroidFullscreenSetup,
            AndroidAudioDevice>()
        );
    return container;
}

AndroidModule::ConfigsPtrT AndroidModule::getSystemConfigs() const {
    return nullptr;
}

void AndroidModule::registerEntityLogics(EntityLogicRegister& logicRegister) const {
}