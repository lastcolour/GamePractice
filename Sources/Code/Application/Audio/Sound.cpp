#include "Audio/Sound.hpp"
#include "Audio/ETAudioInterfaces.hpp"
#include "Audio/SoundSource.hpp"
#include "Audio/OggDataStream.hpp"
#include "ETApplicationInterfaces.hpp"

#include <cassert>

Sound::Sound() :
    soundSource(nullptr) {
}

Sound::~Sound() {
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
    soundSource->attachToController(*this);
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

bool Sound::isPlaying() const {
    if(!soundSource) {
        return false;
    }
    return soundSource->isStreaming();
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

OggDataStream* Sound::getDataStream() {
    return dataStream.get();
}

void Sound::setVolume(float volume) {
    if(!soundSource) {
        return;
    }
    soundSource->setGain(volume);
}