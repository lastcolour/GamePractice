#include "Audio/SoundEvent.hpp"
#include "Audio/ETSoundManagers.hpp"

void SoundEvent::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<SoundEvent>("SoundEvent")) {
        classInfo->addField("event", &SoundEvent::event);
    }
}

SoundEvent::SoundEvent() {
}

SoundEvent::SoundEvent(const SoundEvent& other) :
    event(other.event) {
}

SoundEvent& SoundEvent::operator=(const SoundEvent& other) {
    event = other.event;
    return *this;
}

SoundEvent::~SoundEvent() {
}

void SoundEvent::emit() {
    if(!event.isSet() || event.get().empty()) {
        return;
    }
    ET_SendEvent(&ETSoundEventManager::ET_emitEvent, event.get().c_str());
}