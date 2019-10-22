#include "Audio/Desktop/ALAudioSystem.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Audio/AudioConfig.hpp"

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
    ETNode<ETTimerEvents>::connect(getEntityId());
    ETNode<ETSoundSourceManager>::connect(getEntityId());
    return true;
}

void ALAudioSystem::deinit() {
    if(alcContext) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(alcContext);
        alcContext = nullptr;
    }
    if(alcDevice) {
        alcCloseDevice(alcDevice);
        alcDevice = nullptr;
    }
    ETNode<ETTimerEvents>::disconnect();
    ETNode<ETSoundSourceManager>::disconnect();
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
    const int maxSoundSources = ET_getConfig<AudioConfig>()->maxSoundSources;
    ALuint sourceIds[maxSoundSources];
    alGenSources(maxSoundSources, &sourceIds[0]);
    auto alError = alGetError();

    if(alError != AL_NO_ERROR) {
        LogError("[ALAudioSystem::initSoundSources] Can't %d init audio sources. Error: %s", maxSoundSources, alGetString(alError));
        return false;
    }

    sources.reserve(maxSoundSources);
    sourceStateMap.reserve(maxSoundSources);

    bool initAllSources = true;
    for(int i = 0; i < maxSoundSources; ++i) {
        sources.emplace_back(new ALSoundSource(sourceIds[i]));
        auto& source = sources.back();
        if(!source->init()) {
            initAllSources = false;
        }
        sourceStateMap.emplace_back(ESourceState::Free);
    }

    if(!initAllSources) {
        LogWarning("[ALAudioSystem::initSoundSources] Can't init all audio sources");
        return false;
    }

    return true;
}

SoundSource* ALAudioSystem::ET_getFreeSource() {
    for(int i = 0, sz = sourceStateMap.size(); i < sz; ++i) {
        if(sourceStateMap[i] == ESourceState::Free) {
            sourceStateMap[i] = ESourceState::Busy;
            return sources[i].get();
        }
    }
    return nullptr;
}

void ALAudioSystem::ET_returnSoundSource(SoundSource* retSoundSoruce) {
    assert(retSoundSoruce != nullptr && "Invalid sound source");
    for(int i = 0, sz = sources.size(); i < sz; ++i) {
        auto& source = sources[i];
        if(retSoundSoruce == source.get()) {
            assert(sourceStateMap[i] == ESourceState::Busy && "Try return free source");
            sourceStateMap[i] = ESourceState::Free;
            return;
        }
    }
    assert(false && "Can't find sound source");
}

void ALAudioSystem::ET_onTick(float dt) {
    for(int i = 0, sz = sources.size(); i < sz; ++i) {
        if(sourceStateMap[i] == ESourceState::Busy) {
            auto& source = sources[i];
            source->update();
        }
    }
}