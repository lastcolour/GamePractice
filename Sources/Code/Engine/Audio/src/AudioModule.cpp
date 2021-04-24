#include "Audio/AudioModule.hpp"
#include "SoundEventManager.hpp"
#include "SoundDataManager.hpp"
#include "Logics/SoundPlayLogic.hpp"
#include "Logics/SoundEqualizerLogic.hpp"
#include "Entity/EntityLogicsRegister.hpp"
#include "Core/GlobalData.hpp"
#include "SoundConfig.hpp"

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
            SoundDataManager,
            SoundEventManager,
            AudioSystem>()
        );
    return container;
}

void AudioModule::createSystemConfigs() const {
    CreateGlobal<SoundConfig>("Config/Sound.json");
}

void AudioModule::registerEntityLogics(EntityLogicsRegister& logicsRegister) const {
    logicsRegister.registerLogic<SoundPlayLogic>();
    logicsRegister.registerLogic<SoundEqualizerLogic>();
}
