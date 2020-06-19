#include "Sound.hpp"
#include "Audio/ETAudioInterfaces.hpp"
#include "SoundSource.hpp"
#include "OggDataStream.hpp"
#include "Core/ETLogger.hpp"

#include <cassert>

Sound::Sound(std::unique_ptr<OggDataStream>&& stream) :
    dataStream(std::move(stream)),
    soundSource(nullptr) {

    assert(dataStream && "Invalid data stream");
    dataStream->sound = this;
}

Sound::~Sound() {
    dataStream->sound = nullptr;
    stop();
}

void Sound::play(bool looped) {
    if(soundSource) {
        return;
    }
    ET_SendEventReturn(soundSource, &ETSoundSourceManager::ET_getFreeSource);
    if(!soundSource) {
        LogWarning("[Sound::play] Can't get free sound source to play sound");
        return;
    }
    soundSource->attachToDataStream(*dataStream);
    soundSource->setLoop(looped);
}

void Sound::pause() {
    if(!soundSource) {
        return;
    }
    soundSource->pauseStreaming();
}

void Sound::resume() {
    if(!soundSource) {
        play(soundSource->isLooped());
        return;
    }
    soundSource->resumeStreaming();
}

void Sound::stop() {
    if(!soundSource) {
        return;
    }
    soundSource->stopStreaming();
}

void Sound::detachFromSource() {
    assert(soundSource != nullptr && "Invalid sound source");
    soundSource = nullptr;
}

void Sound::setVolume(float volume) {
    if(!soundSource) {
        return;
    }
    soundSource->setGain(volume);
}

bool Sound::isPlaying() const {
    if(!soundSource) {
        return false;
    }
    return !soundSource->isPaused();
}