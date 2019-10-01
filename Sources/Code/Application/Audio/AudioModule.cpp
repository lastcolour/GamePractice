#include "Audio/AudioModule.hpp"
#include "Audio/AudioSystem.hpp"

AudioModule::AudioModule() :
    SystemModule("Sound") {
}

AudioModule::~AudioModule() {
}

AudioModule::LogicsContainerPtrT AudioModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            AudioSystem>()
        );
    return container;
}