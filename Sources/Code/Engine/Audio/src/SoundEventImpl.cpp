#include "SoundEventImpl.hpp"
#include "Audio/ETSoundPlayManager.hpp"
#include "Core/ETPrimitives.hpp"

#include <cassert>

SoundEventImpl::SoundEventImpl(float volumeVal, float nextDelayVa, Buffer& data) :
    soundData(data),
    volume(volumeVal),
    nextDelay(nextDelayVa),
    lastPlayTime(-1.f) {
}

SoundEventImpl::~SoundEventImpl() {
}

unsigned int SoundEventImpl::getSamplesOffset() const {
    return 0u;
}

void SoundEventImpl::setMixNode(SourceNode* node) {
}

float SoundEventImpl::getMixVolume() const {
    return volume;
}

bool SoundEventImpl::isEvent() const {
    return true;
}

bool SoundEventImpl::isMixLooped() const {
    return false;
}

Buffer& SoundEventImpl::getData() {
    return soundData;
}

void SoundEventImpl::emit() {
    assert(soundData && "Invalid sound data");
    if(lastPlayTime < 0.f) {
        lastPlayTime = -1.f;
    }
    ET_SendEvent(&ETSoundPlayManager::ET_play, this);
}