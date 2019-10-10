#include "Audio/AudioModule.hpp"
#include "Audio/AudioSystem.hpp"
#include "Audio/SoundManager.hpp"

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
