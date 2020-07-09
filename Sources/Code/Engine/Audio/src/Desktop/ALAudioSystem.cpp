#include "Desktop/ALAudioSystem.hpp"
#include "AudioConfig.hpp"
#include "Core/ETLogger.hpp"
#include "Core/ETApplication.hpp"

#include <AL/al.h>
#include <AL/alc.h>

#include <cassert>
#include <algorithm>

namespace {

const unsigned int DEVICE_FRAMERATE = 44100;
const unsigned int DEVICE_CHANNELS = 2;
const unsigned int DEVICE_FORMAT = AL_FORMAT_STEREO16;
const unsigned int AL_BUFFERS = 4;
const float BUFFER_TIME = 0.1f;
const unsigned int SAMPLES_PER_BUFFER = 
    static_cast<unsigned int>((DEVICE_FRAMERATE * BUFFER_TIME) / AL_BUFFERS);

void covertFloatToInt16(int16_t* out, float* source, int samples) {
    for(int i = 0; i < samples; ++i) {
        float fVal = source[i];
        fVal += 1.0;
        fVal *= 32768.0f;
        auto iVal = static_cast<int32_t>(fVal);
        iVal = std::min(65536, std::max(0, iVal));
        iVal -= 32768;
        out[i] = static_cast<int16_t>(iVal);
    }
}

} // namespace

ALAudioSystem::ALAudioSystem() :
    alcDevice(nullptr),
    alcContext(nullptr),
    alSourceId(0),
    alSourcePlaying(false) {
}

ALAudioSystem::~ALAudioSystem() {
}

bool ALAudioSystem::init() {
    if(!initSoundContext()) {
        return false;
    }
    if(!initAlSource()) {
        return false;
    }
    if(!mixGrap.init()) {
        LogError("[ALAudioSystem::init] Can't init mix graph");
        return false;
    }
    ETNode<ETSystemTimerEvents>::connect(getEntityId());
    ETNode<ETSoundPlayManager>::connect(getEntityId());
    return true;
}

void ALAudioSystem::deinit() {
    if(alSourceId) {
        alSourceStop(alSourceId);
        alSourcei(alSourceId, AL_BUFFER, AL_NONE);
        alDeleteSources(1, &alSourceId);
        alDeleteBuffers(AL_BUFFERS, alBufferIds.get());
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

    alBufferIds.reset(new ALuint[AL_BUFFERS]);
    alGenBuffers(AL_BUFFERS, alBufferIds.get());
    alError = alGetError();
    if(alError != AL_NO_ERROR) {
        LogWarning("[ALSoundSource::initAlSource] Can't generate AL buffers for a source (Error: %s)", alGetString(alError));
        return false;
    }

    mixBuffer.reset(new float[SAMPLES_PER_BUFFER * DEVICE_CHANNELS]);
    if(!mixBuffer) {
        LogWarning("[ALSoundSource::initAlSource] Can't allocate mix buffer", alGetString(alError));
        return false;
    }

    std::fill_n(reinterpret_cast<int16_t*>(mixBuffer.get()), SAMPLES_PER_BUFFER * DEVICE_CHANNELS, 0);

    for(auto i = 0; i < AL_BUFFERS; ++i) {
        auto bufferId = (alBufferIds.get())[i];
        alBufferData(bufferId, DEVICE_FORMAT, mixBuffer.get(), 512, DEVICE_FRAMERATE);
        alSourceQueueBuffers(alSourceId, 1, &bufferId);
    }

    alError = alGetError();
    if(alError != AL_NO_ERROR) {
        LogWarning("[ALSoundSource::initAlSource] Can't queuen AL buffers (Error: %s)", alGetString(alError));
        return false;
    }

    return true;
}

void ALAudioSystem::ET_onSystemTick(float dt) {
    if(!alSourcePlaying) {
        alSourcePlay(alSourceId);
        alSourcePlaying = true;
        return;
    }

    ALuint bufferQueue[AL_BUFFERS];
    ALint buffersProcessed = 0;
    alGetSourcei(alSourceId, AL_BUFFERS_PROCESSED, &buffersProcessed);
    if(buffersProcessed == 0) {
        return;
    }
    if(buffersProcessed == AL_BUFFERS) {
        LogWarning("[ALAudioSystem::ET_onSystemTick] All buffers processed!");
    }

    for(ALint i = 0; i < buffersProcessed; ++i) {
        ALuint bufferId;
        alSourceUnqueueBuffers(alSourceId, 1, &bufferId);
        bufferQueue[i] = bufferId;
    }

    for(ALint i = 0; i < buffersProcessed; ++i) {
        auto bufferId = bufferQueue[i];
        mixGrap.mix(mixBuffer.get(), DEVICE_CHANNELS, SAMPLES_PER_BUFFER);
        covertFloatToInt16(reinterpret_cast<int16_t*>(mixBuffer.get()), mixBuffer.get(), SAMPLES_PER_BUFFER * DEVICE_CHANNELS);
        alBufferData(bufferId, DEVICE_FORMAT, mixBuffer.get(),
            SAMPLES_PER_BUFFER * sizeof(int16_t) * DEVICE_CHANNELS, DEVICE_FRAMERATE);
        alSourceQueueBuffers(alSourceId, 1, &bufferId);
    }

    ALenum alSourceState;
    alGetSourcei(alSourceId, AL_SOURCE_STATE, &alSourceState);
    if(alSourceState != AL_PLAYING) {
        alSourcePlay(alSourceId);
    }
}

bool ALAudioSystem::ET_play(SoundStream* soundStream) {
    return mixGrap.playSound(soundStream);
}