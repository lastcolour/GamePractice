#include "Audio/Sound.hpp"
#include "Audio/ETAudioInterfaces.hpp"
#include "Audio/SoundSource.hpp"
#include "Audio/OggDataStream.hpp"
#include "ETApplicationInterfaces.hpp"

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