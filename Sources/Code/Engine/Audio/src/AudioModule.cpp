#include "Audio/AudioModule.hpp"
#include "SoundManager.hpp"
#include "AudioConfig.hpp"
#include "Logics/SoundPlayLogic.hpp"
#include "Entity/EntityLogicsRegister.hpp"

#if defined PLATFORM_WINDOWS || defined PLATFORM_LINUX

  #include "Desktop/ALAudioSystem.hpp"
  using AudioSystem = ALAudioSystem;

#elif defined PLATFORM_ANDROID

  #include "Android/OboeAudioSystem.hpp"
  using AudioSystem = OboeAudioSystem;

#else

    #error Cant determine which audio system need to use

#endif /* DESKTOP_APP_BUILD */

AudioModule::AudioModule() :
    SystemModule("Audio") {
}

AudioModule::~AudioModule() {
}

AudioModule::LogicsContainerPtrT AudioModule::createSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            AudioSystem,
            SoundManager>()
        );
    return container;
}

void AudioModule::reflectSystemConfigs(ReflectContext& ctx) const {
    ctx.reflect<AudioConfig>();
}

void AudioModule::registerEntityLogics(EntityLogicsRegister& logicsRegister) const {
    logicsRegister.registerLogic<SoundPlayLogic>();
}
