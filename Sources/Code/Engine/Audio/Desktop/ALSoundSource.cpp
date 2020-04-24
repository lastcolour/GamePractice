#include "Audio/Desktop/ALSoundSource.hpp"
#include "Audio/OggDataStream.hpp"
#include "Audio/Sound.hpp"
#include "Audio/ETAudioInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"

#include <AL/al.h>
#include <AL/alc.h>

#include <cassert>

namespace {

const int SAMPLES_PER_READ = 44100 / 10;
int16_t* buffData = new int16_t[SAMPLES_PER_READ * 2];

} // namespace 

ALSoundSource::ALSoundSource(unsigned int newSourceId) :
    dataStream(nullptr),
    sourceId(newSourceId),
    state(ESourceState::Ended),
    looping(false) {

    resetALSourceParams();
}

ALSoundSource::~ALSoundSource() {
    alSourceStop(sourceId);
    alSourcei(sourceId, AL_BUFFER, AL_NONE);
    alDeleteSources(1, &sourceId);
    alDeleteBuffers(MAX_BUFFERS_PER_STREAM, &alBufferIds[0]);
}

bool ALSoundSource::init() {
    alGenBuffers(MAX_BUFFERS_PER_STREAM, &alBufferIds[0]);

    ALenum alError = alGetError();
    if(alError != AL_NO_ERROR) {
        LogWarning("[ALSoundSource::init] Can't generate buffers. Error: %s", alGetString(alError));
        return false;
    }

    return true;
}

void ALSoundSource::update() {
    if(state == ESourceState::Ended) {
        return;
    }

    ALuint bufferQueue[MAX_BUFFERS_PER_STREAM];
    ALint buffersProcessed = 0;
    alGetSourcei(sourceId, AL_BUFFERS_PROCESSED, &buffersProcessed);
    for(ALint i = 0; i < buffersProcessed; ++i) {
        ALuint bufferId;
        alSourceUnqueueBuffers(sourceId, 1, &bufferId);
        bufferQueue[i] = bufferId;
    }

    if(state == ESourceState::WaitEnd && buffersProcessed > 0) {
        ALint bufferQueued = 0;
        alGetSourcei(sourceId, AL_BUFFERS_QUEUED, &bufferQueued);
        if(bufferQueued == 0) {
            stopStreaming();
            return;
        }
    }

    if(buffersProcessed > 0) {
        queueALBuffers(&bufferQueue[0], buffersProcessed);
        startALSource();
    }
}

void ALSoundSource::setLoop(bool loopFlag) {
    assert(dataStream != nullptr && "Change loop property with invalid data stream");
    looping = loopFlag;
}

bool ALSoundSource::isLooped() const {
    return looping;
}

bool ALSoundSource::isPaused() const {
    return !isStreaming();
}

void ALSoundSource::setGain(float newGain) {
    alSourcef(sourceId, AL_GAIN, newGain);
}

void ALSoundSource::attachToDataStream(OggDataStream& newDataStream) {
    if(dataStream) {
        assert(false && "Invalind current data stream");
        return;
    }
    assert(state == ESourceState::Ended && "Invalid state of source");

    state = ESourceState::Normal;
    dataStream = &newDataStream;
    dataStream->setSampleOffset(0);

    queueALBuffers(&alBufferIds[0], MAX_BUFFERS_PER_STREAM);
    startALSource();
}

void ALSoundSource::pauseStreaming() {
    if(isStreaming()) {
        alSourcePause(sourceId);
    }
}

void ALSoundSource::resumeStreaming() {
    if(!isStreaming()) {
        alSourcePlay(sourceId);
    }
}

void ALSoundSource::stopStreaming() {
    alSourceStop(sourceId);
    alSourcei(sourceId, AL_BUFFER, AL_NONE);

    assert(dataStream != nullptr && "Invalid controller");
    if(dataStream->sound) {
        dataStream->sound->detachFromSource();
    }
    dataStream = nullptr;
    looping = false;
    state = ESourceState::Ended;
    resetALSourceParams();

    ET_SendEvent(&ETSoundSourceManager::ET_returnSoundSource, this);
}

bool ALSoundSource::isStreaming() const {
    ALenum sourceState;
    alGetSourcei(sourceId, AL_SOURCE_STATE, &sourceState);
    return sourceState == AL_PLAYING;
}

void ALSoundSource::startALSource() {
    if(!isStreaming()) {
        alSourcePlay(sourceId);

        ALenum alError = alGetError();
        if(alError != AL_NO_ERROR) {
            LogWarning("[SoundSource::startALSource] Can't start stream. Error: %s", alGetString(alError));
        }
    }
}

ALSoundSource::EBufferFillRes ALSoundSource::fillALBuffer(unsigned int bufferId) {
    assert(dataStream != nullptr && "Invalid data stream");

    int channels = dataStream->channels;
    int dataFormat = AL_FORMAT_MONO16;
    if(channels == 2) {
        dataFormat = AL_FORMAT_STEREO16;
    }

    EBufferFillRes fillRes = EBufferFillRes::Normal;
    int fillCount = dataStream->fillI16(buffData, SAMPLES_PER_READ, channels, looping);
    if(fillCount < SAMPLES_PER_READ) {
        fillRes = EBufferFillRes::EndOfStream;
    }

    alBufferData(bufferId, dataFormat, &buffData[0], fillCount * sizeof(int16_t) * channels, dataStream->sampleRate);
    alSourceQueueBuffers(sourceId, 1, &bufferId);

    return fillRes;
}

void ALSoundSource::queueALBuffers(unsigned int* bufferIds, int size) {
    for(int i = 0; i < size; ++i) {
        ALuint bufferId = bufferIds[i];
        auto fillRes = fillALBuffer(bufferId);
        if(fillRes == EBufferFillRes::EndOfStream) {
            state = ESourceState::WaitEnd;
            break;
        }
    }

    ALenum alError = alGetError();
    if(alError != AL_NO_ERROR) {
        LogWarning("[ALSoundSource::queueBuffers] Can't queue buffers. Error: %s", alGetString(alError));
    }
}

void ALSoundSource::resetALSourceParams() {
    alSourcef(sourceId, AL_GAIN, 1.f);
    alSourcef(sourceId, AL_PITCH, 1.f);
}