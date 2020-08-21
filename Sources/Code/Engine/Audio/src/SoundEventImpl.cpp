#include "SoundEventImpl.hpp"
#include "Audio/ETSoundPlayManager.hpp"
#include "Core/ETPrimitives.hpp"

#include <cassert>

SoundEventImpl::SoundEventImpl(float volumeVal, float nextDelayVal, Buffer& data) :
    soundData(data),
    volume(volumeVal),
    nextDelay(nextDelayVal) {

    assert(soundData && "Invalid sound data");
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
    TimePoint frameStartT;
    if(frameStartT.getSecondsElapsedFrom(lastPlayTime) < (nextDelay + 0.001f)) {
        return;
    }
    lastPlayTime = frameStartT;
    ET_SendEvent(&ETSoundPlayManager::ET_play, this);
}