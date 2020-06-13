#include "Audio/Desktop/ALAudioSystem.hpp"
#include "Audio/Desktop/ALSoundSource.hpp"
#include "Audio/AudioConfig.hpp"
#include "Core/ETLogger.hpp"
#include "Core/ETApplication.hpp"

#include <AL/al.h>
#include <AL/alc.h>

#include <cassert>

ALAudioSystem::ALAudioSystem() :
    alcDevice(nullptr),
    alcContext(nullptr) {
}

ALAudioSystem::~ALAudioSystem() {
}

bool ALAudioSystem::init() {
    if(!initSoundContext()) {
        return false;
    }
    if(!initSoundSources()) {
        return false;
    }
    ETNode<ETAudioSystem>::connect(getEntityId());
    return true;
}

void ALAudioSystem::deinit() {
    ETNode<ETAudioSystem>::disconnect();
    alSources.clear();
    if(alcContext) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(alcContext);
        alcContext = nullptr;
    }
    if(alcDevice) {
        alcCloseDevice(alcDevice);
        alcDevice = nullptr;
    }
}

bool ALAudioSystem::initSoundContext() {
    alcDevice = alcOpenDevice(nullptr);
    if(!alcDevice) {
        LogError("[ALAudioSystem::initSoundContext] Can't open default ALC device");
        return false;
    }
    alcContext = alcCreateContext(alcDevice, nullptr);
    if(!alcContext) {
        LogError("[ALAudioSystem::initSoundContext] Can't create audio context. Error: %s", alcGetString(alcDevice, alcGetError(alcDevice)));
        return false;
    }
    ALCboolean res = alcMakeContextCurrent(alcContext);
    if(res != ALC_TRUE) {
        LogError("[ALAudioSystem::initSoundContext] Can't make context current. Error: %s", alcGetString(alcDevice, alcGetError(alcDevice)));
        return false;
    }
    return true;
}

bool ALAudioSystem::initSoundSources() {
    auto maxSources = ET_getConfig<AudioConfig>()->maxSoundSources;

    std::unique_ptr<ALuint[]> sourceIds(new ALuint[maxSources]);
    alGenSources(maxSources, &sourceIds[0]);
    auto alError = alGetError();
    if(alError != AL_NO_ERROR) {
        LogError("[ALAudioSystem::initSoundSources] Can't %d init audio sources. Error: %s", maxSources, alGetString(alError));
        return false;
    }

    for(int i = 0; i < maxSources; ++i) {
        std::unique_ptr<ALSoundSource> alSoundSource(new ALSoundSource(sourceIds[i]));
        if(!alSoundSource->init()) {
            alSources.clear();
            return false;
        }
        alSources.emplace_back(std::move(alSoundSource));
    }

    return true;
}

std::vector<SoundSource*> ALAudioSystem::ET_getSourcesToManage() {
    std::vector<SoundSource*> sources;
    for(size_t i = 0, sz = alSources.size(); i < sz; ++i) {
        sources.emplace_back(alSources[i].get());
    }
    return sources;
}