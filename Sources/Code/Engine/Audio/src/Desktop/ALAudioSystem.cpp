#include "Desktop/ALAudioSystem.hpp"
#include "Core/ETApplication.hpp"
#include "SoundConfig.hpp"

#include <AL/al.h>
#include <AL/alc.h>

#include <cassert>

ALAudioSystem::ALAudioSystem() :
    alcDevice(nullptr),
    alcContext(nullptr),
    alSourceId(0),
    alSourcePlaying(false) {
}

ALAudioSystem::~ALAudioSystem() {
}

bool ALAudioSystem::init() {
    if(!mixGraph.init()) {
        LogError("[ALAudioSystem::init] Can't init mix graph");
        return false;
    }
    if(!initSoundContext()) {
        return false;
    }
    if(!initAlSource()) {
        return false;
    }
    ETNode<ETSoundUpdateTask>::connect(getEntityId());
    ETNode<ETAudioSystem>::connect(getEntityId());
    ETNode<ETSoundPlayManager>::connect(getEntityId());
    return true;
}

void ALAudioSystem::deinit() {
    auto& mixConfig = mixGraph.getMixConfig();
    if(alSourceId) {
        alSourceStop(alSourceId);
        alSourcei(alSourceId, AL_BUFFER, AL_NONE);
        alDeleteSources(1, &alSourceId);
        alDeleteBuffers(mixConfig.buffersCount, alBufferIds.get());
        alSourceId = 0;
    }
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

bool ALAudioSystem::initAlSource() {
    alGenSources(1, &alSourceId);
    ALenum alError = alGetError();
    if(alError != AL_NO_ERROR) {
        LogWarning("[ALSoundSource::initAlSource] Can't crate AL source (Error: %s)", alGetString(alError));
        return false;
    }

    alSourcef(alSourceId, AL_GAIN, 1.f);
    alSourcef(alSourceId, AL_PITCH, 1.f);
    alSource3f(alSourceId, AL_POSITION, 0.f, 0.f, 0.f);
    alSource3f(alSourceId, AL_VELOCITY, 0.f, 0.f, 0.f);

    auto& mixConfig = mixGraph.getMixConfig();

    alBufferIds.reset(new ALuint[mixConfig.buffersCount]);
    queueBufferIds.reset(new ALuint[mixConfig.buffersCount]);
    alGenBuffers(mixConfig.buffersCount, alBufferIds.get());
    alError = alGetError();
    if(alError != AL_NO_ERROR) {
        LogWarning("[ALSoundSource::initAlSource] Can't generate AL buffers for a source (Error: %s)", alGetString(alError));
        return false;
    }

    mixBuffer.reset(new float[mixConfig.samplesPerBuffer * mixConfig.channels]);
    if(!mixBuffer) {
        LogWarning("[ALSoundSource::initAlSource] Can't allocate mix buffer");
        return false;
    }

    std::fill_n(reinterpret_cast<int16_t*>(mixBuffer.get()), mixConfig.samplesPerBuffer * mixConfig.channels,
        static_cast<int16_t>(0));

    auto alFormat = mixConfig.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

    for(auto i = 0; i < mixConfig.buffersCount; ++i) {
        auto bufferId = (alBufferIds.get())[i];
        alBufferData(bufferId, alFormat, mixBuffer.get(),
            mixConfig.samplesPerBuffer * mixConfig.channels * sizeof(int16_t), mixConfig.outSampleRate);
        alSourceQueueBuffers(alSourceId, 1, &bufferId);
    }

    alError = alGetError();
    if(alError != AL_NO_ERROR) {
        LogWarning("[ALSoundSource::initAlSource] Can't queuen AL buffers (Error: %s)", alGetString(alError));
        return false;
    }

    return true;
}

void ALAudioSystem::ET_updateSound() {
    ET_PollAllEvents<ETAudioSystem>();
    ET_PollAllEvents<ETSoundDataManager>();

    ET_SendEvent(&ETSoundDataManager::ET_updateSoundData);

    updateMixGraph();
}

void ALAudioSystem::updateMixGraph() {
    auto& mixConfig = mixGraph.getMixConfig();

    if(!alSourcePlaying) {
        alSourcePlay(alSourceId);
        alSourcePlaying = true;
        return;
    }

    ALint buffersProcessed = 0;
    alGetSourcei(alSourceId, AL_BUFFERS_PROCESSED, &buffersProcessed);
    if(buffersProcessed == 0) {
        return;
    }
    if(buffersProcessed == mixConfig.buffersCount) {
        LogWarning("[ALAudioSystem::updateMixGraph] All buffers processed!");
    }

    alSourceUnqueueBuffers(alSourceId, buffersProcessed, &queueBufferIds[0]);

    auto alFormat = mixConfig.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    for(ALint i = 0; i < buffersProcessed; ++i) {
        auto bufferId = (queueBufferIds.get())[i];
        mixGraph.mixBufferAndConvert(mixBuffer.get());
        alBufferData(bufferId, alFormat, mixBuffer.get(),
            mixConfig.samplesPerBuffer * mixConfig.channels * sizeof(int16_t), mixConfig.outSampleRate);
    }

    alSourceQueueBuffers(alSourceId, buffersProcessed, &queueBufferIds[0]);

    ALenum alSourceState;
    alGetSourcei(alSourceId, AL_SOURCE_STATE, &alSourceState);
    if(alSourceState != AL_PLAYING) {
        alSourcePlay(alSourceId);
    }
}

void ALAudioSystem::ET_setEqualizer(ESoundGroup soundGroup, const EqualizerSetup& eqSetup) {
    mixGraph.setEqualizer(soundGroup, eqSetup);
}

bool ALAudioSystem::ET_startSound(SoundProxy& proxyNode, float duration, bool isEvent) {
    return mixGraph.startSound(proxyNode, duration, isEvent);
}

void ALAudioSystem::ET_stopSound(SoundProxy& proxyNode, float duration, bool resetOffset) {
    return mixGraph.stopSound(proxyNode, duration, resetOffset);
}

void ALAudioSystem::ET_setMasterVolume(float newVolume) {
    mixGraph.setMasterVolume(newVolume);
}

float ALAudioSystem::ET_getMasterVolume() const {
    return mixGraph.getMasterVolume();
}