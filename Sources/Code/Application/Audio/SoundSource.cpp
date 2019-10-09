#include "Audio/SoundSource.hpp"
#include "Audio/OggDataStream.hpp"
#include "ETApplicationInterfaces.hpp"

#include <AL/al.h>
#include <AL/alc.h>

#include <cassert>

namespace {

const int SAMPLES_PER_READ = 44100 / 10;
int16_t* buffData = new int16_t[SAMPLES_PER_READ];

} // namespace 

SoundSource::SoundSource(unsigned int newSourceId) :
    dataStream(nullptr),
    sourceId(newSourceId),
    looping(true),
    ended(false),
    samplesOffset(0) {

    alSourcef(sourceId, AL_GAIN, 1.f);
    alSourcef(sourceId, AL_PITCH, 1.f);
}

SoundSource::~SoundSource() {
    stopStreaming();
    alDeleteSources(1, &sourceId);
    alDeleteBuffers(MAX_BUFFERS_PER_STREAM, &alBufferIds[0]);
}

bool SoundSource::init() {
    alGenBuffers(MAX_BUFFERS_PER_STREAM, &alBufferIds[0]);

    ALenum alError = alGetError();
    if(alError != AL_NO_ERROR) {
        LogWarning("[SoundSource::SoundSource] Can't generate buffers. Error: %s", alGetString(alError));
        return false;
    }

    return true;
}

void SoundSource::reset() {
    dataStream = nullptr;
    looping = true;
    ended = false;
}

void SoundSource::startStreaming(OggDataStream& newDataStream) {
    if(dataStream) {
        assert(false && "Invalind current data stream");
        return;
    }

    dataStream = &newDataStream;
    queueALBuffers(&alBufferIds[0], MAX_BUFFERS_PER_STREAM);
    startALSource();
}

void SoundSource::updateStreaming() {
    if(ended) {
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

    if(buffersProcessed > 0) {
        queueALBuffers(&bufferQueue[0], buffersProcessed);
        startALSource();
    }
}

void SoundSource::pauseStreaming() {
    if(isStreaming()) {
        alSourcePause(sourceId);
    }
}

void SoundSource::resumeStreaming() {
    if(!isStreaming()) {
        alSourcePlay(sourceId);
    }
}

void SoundSource::stopStreaming() {
    alSourceStop(sourceId);
    alSourcei(sourceId, AL_BUFFER, AL_NONE);
    dataStream = nullptr;
}

bool SoundSource::isStreaming() const {
    ALenum sourceState;
    alGetSourcei(sourceId, AL_SOURCE_STATE, &sourceState);
    return sourceState == AL_PLAYING;
}

void SoundSource::startALSource() {
    if(!isStreaming()) {
        alSourcePlay(sourceId);

        ALenum alError = alGetError();
        if(alError != AL_NO_ERROR) {
            LogWarning("[SoundSource::startALSource] Can't start stream. Error: %s", alGetString(alError));
        }
    }
}

SoundSource::EBufferFillRes SoundSource::fillALBuffer(unsigned int bufferId) {
    int dataFormat = AL_FORMAT_MONO16;
    if (dataStream->channels == 2) {
        dataFormat = AL_FORMAT_STEREO16;
    }

    EBufferFillRes fillRes = EBufferFillRes::Normal;

    int readSamplesCount = dataStream->readSamples(buffData, SAMPLES_PER_READ);
    if(readSamplesCount < SAMPLES_PER_READ) {
        if(!looping) {
            fillRes = EBufferFillRes::EndOfStream;
        } else {
            while (SAMPLES_PER_READ - readSamplesCount > 0) {
                dataStream->setSampleOffset(0);
                readSamplesCount += dataStream->readSamples(static_cast<int16_t*>(buffData) + readSamplesCount, SAMPLES_PER_READ - readSamplesCount);
            }
        }
    }

    alBufferData(bufferId, dataFormat, &buffData[0], readSamplesCount * sizeof(int16_t), dataStream->sampleRate);
    alSourceQueueBuffers(sourceId, 1, &bufferId);

    return fillRes;
}

void SoundSource::queueALBuffers(unsigned int* bufferIds, int size) {
    for(size_t i = 0; i < size; ++i) {
        ALuint bufferId = bufferIds[i];
        auto fillRes = fillALBuffer(bufferId);
        if(fillRes == EBufferFillRes::EndOfStream) {
            ended = true;
            break;
        }
    }

    ALenum alError = alGetError();
    if(alError != AL_NO_ERROR) {
        LogWarning("[SoundSource::queueBuffers] Can't queue buffers. Error: %s", alGetString(alError));
    }
}