#include "Audio/SoundSource.hpp"
#include "Audio/OggDataStream.hpp"
#include "ETApplicationInterfaces.hpp"

#include <AL/al.h>
#include <AL/alc.h>

#include <cassert>

namespace {

const int BUFFER_SIZE_PER_STREAM = 16 * 16;

} // namespace 

SoundSource::SoundSource(int newSourceId) :
    dataStream(nullptr),
    sourceId(newSourceId),
    looping(false) {

    alSourcef(sourceId, AL_GAIN, 1.f);
    alSourcef(sourceId, AL_PITCH, 1.f);
    alSourcei(sourceId, AL_LOOPING, looping);

    alGenBuffers(MAX_BUFFERS_PER_STREAM, &(buffers[0]));
}

SoundSource::~SoundSource() {
    stopStreaming();
    alDeleteBuffers(MAX_BUFFERS_PER_STREAM, &(buffers[0]));
}

void SoundSource::startStreaming(OggDataStream* newDataStream) {
    if(dataStream) {
        assert(false && "Invalind current data stream");
        return;
    }
    if(!newDataStream) {
        assert(false && "Invalid new data stream");
        return;
    }
    dataStream = newDataStream;
    std::vector<unsigned int> bufferQueue;
    for(int i=0; i < MAX_BUFFERS_PER_STREAM; ++i) {
        bufferQueue.push_back(buffers[i]);
    }

    queueBuffers(bufferQueue);

    startPlay();
}

void SoundSource::startPlay() {
    ALenum sourceState;
    alGetSourcei(sourceId, AL_SOURCE_STATE, &sourceState);
    if(sourceState != AL_PLAYING) {
        alSourcePlay(sourceId);

        auto alError = alGetError();
        if(alError != AL_NO_ERROR) {
            LogWarning("[SoundSource::startPlay] Can't start stream. Error: %s", alGetString(alError));
        }
    }
}

void SoundSource::queueBuffers(const std::vector<unsigned int>& bufferQueue) {
    for(size_t i = 0, sz = bufferQueue.size(); i < sz; ++i) {
        ALuint bufferId = bufferQueue[i];

        int dataFormat = AL_FORMAT_MONO16;
        if(dataStream->channels == 1) {
            dataFormat = AL_FORMAT_MONO16;
        } else {
            dataFormat = AL_FORMAT_STEREO16;
        }
        int *dataPtr = reinterpret_cast<int*>(this);
        int buffSize = BUFFER_SIZE_PER_STREAM;
        int sampleRate = dataStream->sampleRate;

        alBufferData(bufferId, dataFormat, dataPtr, buffSize, sampleRate);
        alSourceQueueBuffers(sourceId, 1, &bufferId);
    }
}

void SoundSource::updateStreaming() {
    ALint buffersProcessed;
    alGetSourcei(sourceId, AL_BUFFERS_PROCESSED, &buffersProcessed);
    std::vector<ALuint> bufferQueue;
    for(ALint i = 0; i < buffersProcessed; ++i) {
        ALuint bufferId;
        alSourceUnqueueBuffers(sourceId, 1, &bufferId);
        bufferQueue.push_back(bufferId);
    }

    if(!bufferQueue.empty()) {
        queueBuffers(bufferQueue);
        startPlay();
    }
}

void SoundSource::pauseStreaming() {
    ALenum sourceState;
    alGetSourcei(sourceId, AL_SOURCE_STATE, &sourceState);
    if(sourceState == AL_PLAYING) {
        alSourcePause(sourceId);
    }
}

void SoundSource::stopStreaming() {
    alSourceStop(sourceId);
    alSourcei(sourceId, AL_BUFFER, AL_NONE);
    dataStream = nullptr;
}

void SoundSource::resumeStreaming() {
    ALenum sourceState;
    alGetSourcei(sourceId, AL_SOURCE_STATE, &sourceState);
    if(sourceState != AL_PLAYING) {
        alSourcePlay(sourceId);
    }
}

bool SoundSource::isStreaming() const {
    ALenum sourceState;
    alGetSourcei(sourceId, AL_SOURCE_STATE, &sourceState);
    return sourceState == AL_PLAYING;
}

bool SoundSource::isFinishStreaming() const {
    ALenum sourceState;
    alGetSourcei(sourceId, AL_SOURCE_STATE, &sourceState);
    if(sourceState == AL_STOPPED) {
        return true;
    }
    return false;
}

void SoundSource::reset() {
}