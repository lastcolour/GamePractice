#include "SoundStream.hpp"
#include "OggDataStream.hpp"
#include "Core/ETLogger.hpp"
#include "Audio/ETSoundPlayManager.hpp"
#include "MixGraph/SourceNode.hpp"

#include <cassert>

SoundStream::SoundStream(std::unique_ptr<OggDataStream>&& oggDataStream) :
    oggData(std::move(oggDataStream)),
    mixNode(nullptr),
    volume(1.f),
    state(SoundStreamState::Stop),
    looped(false) {

    assert(oggData && "Invalid OGG data stream");
}

SoundStream::~SoundStream() {
    stop();
}

void SoundStream::exclusiveMixTo(void* out, int channels, int samples) {
    assert(isPlaying() && "Invalid sound stream state");
    if(state == SoundStreamState::MixLast) {
        state = SoundStreamState::Stop;
        oggData->setSampleOffset(0);
        freeMixNode();
        return;
    }
    auto readSamples = oggData->readF32(out, channels, samples, looped);
    if(readSamples < samples) {
        state = SoundStreamState::MixLast;
    }
}

int SoundStream::getChannels() const {
    return oggData->getChannels();
}

int SoundStream::getSampleRate() const {
    return oggData->getSampleRate();
}

void SoundStream::play() {
    if(isPlaying()) {
        return;
    }
    bool res = false;
    ET_SendEventReturn(res, &ETSoundPlayManager::ET_play, this);
    if(res) {
        state = SoundStreamState::Play;
    }
}

void SoundStream::stop() {
    if(state == SoundStreamState::Stop) {
        return;
    }
    state = SoundStreamState::Stop;
    oggData->setSampleOffset(0);
    freeMixNode();
}

void SoundStream::pause() {
    if(state != SoundStreamState::Play) {
        LogWarning("[SoundStream::pause] Can't pause non-playing sound");
        return;
    }
    state = SoundStreamState::Pause;
    freeMixNode();
}

void SoundStream::resume() {
    if(state != SoundStreamState::Pause) {
        return;
    }
    play();
}

void SoundStream::setLooped(bool flag) {
    looped = flag;
}

bool SoundStream::isLooped() const {
    return looped;
}

void SoundStream::setVolume(float newVolume) {
    volume = newVolume;
}

float SoundStream::getVolume() const {
    return volume;
}

bool SoundStream::isPlaying() const {
    return state == SoundStreamState::Play || state == SoundStreamState::MixLast;
}

bool SoundStream::isPaused() const {
    return state == SoundStreamState::Pause;
}

void SoundStream::setMixNode(SourceNode* node) {
    mixNode = node;
}

void SoundStream::freeMixNode() {
    if(mixNode) {
        mixNode->detachFromSource();
        mixNode = nullptr;
    }
}