#include "Audio/SoundEvent.hpp"
#include "Audio/ETSoundManagers.hpp"

void SoundEvent::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<SoundEvent>("SoundEvent")) {
        classInfo->addResourceField("event", ResourceType::SoundEvent, &SoundEvent::eventName);
    }
}

SoundEvent::SoundEvent() {
}

SoundEvent::SoundEvent(const SoundEvent& other) :
    eventName(other.eventName) {
}

SoundEvent& SoundEvent::operator=(const SoundEvent& other) {
    eventName = other.eventName;
    return *this;
}

SoundEvent::~SoundEvent() {
}

void SoundEvent::emit() {
    if(eventName.empty()) {
        return;
    }
    ET_SendEvent(&ETSoundEventManager::ET_emitEvent, eventName.c_str());
}

bool SoundEvent::isValid() const {
    return false;
}