#include "Audio/AudioModule.hpp"
#include "Audio/SoundManager.hpp"
#include "Audio/SoundSourceManager.hpp"
#include "Audio/AudioConfig.hpp"

#if defined PLATFORM_WINDOWS || defined PLATFORM_LINUX

  #include "Audio/Desktop/ALAudioSystem.hpp"
  using AudioSystem = ALAudioSystem;

#elif defined PLATFORM_ANDROID

  #include "Audio/Android/OboeAudioSystem.hpp"
  using AudioSystem = OboeAudioSystem;

#else

    #error Cant determine which audio system need to use

#endif /* DESKTOP_APP_BUILD */

AudioModule::AudioModule() :
    SystemModule("Audio") {
}

AudioModule::~AudioModule() {
}

AudioModule::LogicsContainerPtrT AudioModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            AudioSystem,
            SoundSourceManager,
            SoundManager>()
        );
    return container;
}

AudioModule::ConfigsPtrT AudioModule::getSystemConfigs() const {
    ConfigsPtrT config(new SystemModuleConfig<AudioConfig>());
    return config;
}
