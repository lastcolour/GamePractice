#include "Audio/AudioSystem.hpp"
#include "ETApplicationInterfaces.hpp"

#include <AL/al.h>
#include <AL/alc.h>

#include <cassert>

namespace {

const int MAX_SOUND_SOURCES = 16;

} // namespace

AudioSystem::AudioSystem() :
    alcDevice(nullptr),
    alcContext(nullptr) {
}

AudioSystem::~AudioSystem() {
}

bool AudioSystem::init() {
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

void AudioSystem::deinit() {
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

bool AudioSystem::initSoundContext() {
    alcDevice = alcOpenDevice(nullptr);
    if(!alcDevice) {
        LogError("[AudioSystem::init] Can't open default ALC device");
        return false;
    }
    alcContext = alcCreateContext(alcDevice, nullptr);
    if(!alcContext) {
        LogError("[AudioSystem::init] Can't create audio context. Error: %s", alcGetString(alcDevice, alcGetError(alcDevice)));
        return false;
    }
    ALCboolean res = alcMakeContextCurrent(alcContext);
    if(res != ALC_TRUE) {
        LogError("[AudioSystem::init] Can't make context current. Error: %s", alcGetString(alcDevice, alcGetError(alcDevice)));
        return false;
    }
    return true;
}

bool AudioSystem::initSoundSources() {
    ALuint sourceIds[MAX_SOUND_SOURCES];
    alGenSources(MAX_SOUND_SOURCES, &sourceIds[0]);
    auto alError = alGetError();

    if(alError != AL_NO_ERROR) {
        LogError("[AudioSystem::initSoundSources] Can't %d init audio sources. Error: %s", MAX_SOUND_SOURCES, alGetString(alError));
        return false;
    }

    sources.reserve(MAX_SOUND_SOURCES);
    sourceStateMap.reserve(MAX_SOUND_SOURCES);

    bool initAllSources = true;
    for(int i = 0; i < MAX_SOUND_SOURCES; ++i) {
        sources.emplace_back(sourceIds[i]);
        auto& source = sources.back();
        if(!source.init()) {
            initAllSources = false;
        }
        sourceStateMap.emplace_back(ESourceState::Free);
    }

    if(!initAllSources) {
        LogWarning("[AudioSystem::initSoundSources] Can't init all audio sources");
        return false;
    }

    return true;
}

SoundSource* AudioSystem::ET_getFreeSoundSource() {
    for(int i = 0; i < MAX_SOUND_SOURCES; ++i) {
        if(sourceStateMap[i] == ESourceState::Free) {
            sourceStateMap[i] = ESourceState::Busy;
            return &(sources[i]);
        }
    }
    return nullptr;
}

void AudioSystem::ET_returnSoundSource(SoundSource* retSoundSoruce) {
    assert(retSoundSoruce != nullptr && "Invalid sound source");
    for(int i = 0; i < MAX_SOUND_SOURCES; ++i) {
        auto& source = sources[i];
        if(retSoundSoruce == &source) {
            assert(sourceStateMap[i] == ESourceState::Busy && "Try return free source");
            sourceStateMap[i] = ESourceState::Free;
            source.reset();
            return;
        }
    }
    assert(false && "Can't find sound source");
}

void AudioSystem::ET_onTick(float dt) {
    for(int i = 0; i < MAX_SOUND_SOURCES; ++i) {
        if(sourceStateMap[i] == ESourceState::Busy) {
            auto& source = sources[i];
            source.updateStreaming();
        }
    }
}