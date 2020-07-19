#include "SoundImpl.hpp"
#include "Core/ETLogger.hpp"
#include "Audio/ETSoundPlayManager.hpp"
#include "MixGraph/SourceNode.hpp"

#include <cassert>

SoundImpl::SoundImpl(Buffer& data) :
    soundData(data),
    mixNode(nullptr),
    volume(1.f),
    samplesOffset(0),
    looped(false) {

    assert(soundData && "Invalid sound data");
}

SoundImpl::~SoundImpl() {
    stop();
}

unsigned int SoundImpl::getSamplesOffset() const {
    return samplesOffset;
}

void SoundImpl::setMixNode(SourceNode* node) {
    if(mixNode) {
        assert(!node && "Can't change a mix node");
        samplesOffset = mixNode->getSamplesOffset();
        mixNode = nullptr;
    } else {
        assert(node && "Can't detach from a mix without attach");
        mixNode = node;
    }
}

float SoundImpl::getMixVolume() const {
    return getVolume();
}

bool SoundImpl::isEvent() const {
    return false;
}

bool SoundImpl::isMixLooped() const {
    return isLooped();
}

Buffer& SoundImpl::getData() {
    return soundData;
}

void SoundImpl::play() {
    if(isPlaying()) {
        return;
    }
    assert(soundData && "Invalid sound data");
    ET_SendEvent(&ETSoundPlayManager::ET_play, this);
}

void SoundImpl::stop() {
    if(mixNode) {
        samplesOffset = 0;
        mixNode->detachFromStream();
    }
}

void SoundImpl::pause() {
    if(mixNode) {
        mixNode->detachFromStream();
    }
}

void SoundImpl::resume() {
    if(mixNode) {
        return;
    }
    assert(soundData && "Invalid sound data");
    ET_SendEvent(&ETSoundPlayManager::ET_play, this);
}

void SoundImpl::setLooped(bool flag) {
    looped = flag;
}

bool SoundImpl::isLooped() const {
    return looped;
}

void SoundImpl::setVolume(float newVolume) {
    volume = newVolume;
}

float SoundImpl::getVolume() const {
    return volume;
}

bool SoundImpl::isPlaying() const {
    if(mixNode) {
        return true;
    }
    return false;
}

bool SoundImpl::isPaused() const {
    if(mixNode) {
        return false;
    }
    return samplesOffset > 0;
}