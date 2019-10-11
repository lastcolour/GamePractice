#include "Audio/AudioModule.hpp"
#include "Audio/SoundManager.hpp"

#if defined APP_BUILD_PLATFORM_WINDOWS || defined APP_BUILD_PLATFORM_LINUX

  #include "Audio/Desktop/ALAudioSystem.hpp"
  using AudioSystem = ALAudioSystem;

#elif defined APP_BUILD_PLATFORM_ANDROID

  #include "Audio/Android/OboeAudioSystem.hpp"
  using AudioSystem = OboeAudioSystem;

#else

    #error Can't determine which audio system need to use

#endif /* DESKTOP_APP_BUILD */

AudioModule::AudioModule() :
    SystemModule("Sound") {
}

AudioModule::~AudioModule() {
}

AudioModule::LogicsContainerPtrT AudioModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            AudioSystem,
            SoundManager>()
        );
    return container;
}

AudioModule::ConfigsPtrT AudioModule::getSystemConfigs() const {
    return nullptr;
}
