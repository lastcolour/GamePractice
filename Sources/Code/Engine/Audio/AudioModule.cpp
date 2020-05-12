#include "Audio/AudioModule.hpp"
#include "Audio/SoundManager.hpp"
#include "Audio/SoundSourceManager.hpp"
#include "Audio/AudioConfig.hpp"
#include "Audio/Logics/SoundPlayLogic.hpp"
#include "Entity/EntityLogicRegister.hpp"
#include "Reflect/ReflectContext.hpp"

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

AudioModule::LogicsContainerPtrT AudioModule::createSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            AudioSystem,
            SoundSourceManager,
            SoundManager>()
        );
    return container;
}

void AudioModule::reflectSystemConfigs(ReflectContext& ctx) const {
    if(auto classInfo = ctx.classInfo<AudioModule>("AudioModule")) {
        classInfo->addBaseClass<AudioConfig>();
    }
}

void AudioModule::registerEntityLogics(EntityLogicRegister& logicRegister) const {
    logicRegister.registerLogic<SoundPlayLogic>("SoundPlayer");
}
