#include "Audio/Sound.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Audio/ETAudioInterfaces.hpp"
#include "Audio/SoundSource.hpp"

Sound::Sound() :
    soundSource(nullptr) {
}

Sound::~Sound() {
    stop();
}

void Sound::play() {
    ET_SendEventReturn(soundSource, &ETSoundSourceManager::ET_getFreeSoundSource);
    if(!soundSource) {
        LogWarning("[Sound::play] Can't get free sound source to play sound");
        return;
    }
    soundSource->startStreaming(dataStream.get());
}

void Sound::pause() {
    if(!soundSource) {
        return;
    }
    soundSource->pauseStreaming();
}

void Sound::resume() {
    if(!soundSource) {
        play();
        return;
    }
    soundSource->resumeStreaming();
}

void Sound::stop() {
    if(!soundSource) {
        return;
    }
    soundSource->stopStreaming();
    ET_SendEvent(&ETSoundSourceManager::ET_returnSoundSource, soundSource);
    soundSource = nullptr;
}

bool Sound::isPlaying() const {
    if(!soundSource) {
        return false;
    }
    return soundSource->isStreaming();
}
