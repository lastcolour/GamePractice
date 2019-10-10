#include "Audio/SoundSource.hpp"
#include "Audio/OggDataStream.hpp"
#include "Audio/SoundSourceController.hpp"
#include "Audio/ETAudioInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"

#include <AL/al.h>
#include <AL/alc.h>

#include <cassert>

namespace {

const int SAMPLES_PER_READ = 44100 / 10;
int16_t* buffData = new int16_t[SAMPLES_PER_READ];

} // namespace 

SoundSource::SoundSource(unsigned int newSourceId) :
    controller(nullptr),
    sourceId(newSourceId),
    state(ESourceState::Ended),
    looping(false) {

    resetALSourceParams();
}

SoundSource::~SoundSource() {
    alSourceStop(sourceId);
    alSourcei(sourceId, AL_BUFFER, AL_NONE);
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

void SoundSource::update() {
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

void SoundSource::setLoop(bool loopFlag) {
    assert(controller != nullptr && "Change loop property with invalid controller");
    looping = loopFlag;
}

bool SoundSource::isLooped() const {
    return looping;
}

void SoundSource::setGain(float newGain) {
    assert(isStreaming() && "Set gain for invalid source");

    alSourcef(sourceId, AL_GAIN, newGain);
}

void SoundSource::attachToController(SoundSourceController& newController) {
    if(controller) {
        assert(false && "Invalind current stream controller");
        return;
    }
    assert(state == ESourceState::Ended && "Invalid state of source");

    controller = &newController;
    state = ESourceState::Normal;
    controller->getDataStream()->setSampleOffset(0);

    queueALBuffers(&alBufferIds[0], MAX_BUFFERS_PER_STREAM);
    startALSource();
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

    assert(controller != nullptr && "Invalid controller");
    controller->detachFromSource();
    controller = nullptr;
    looping = false;
    state = ESourceState::Ended;
    resetALSourceParams();

    ET_SendEvent(&ETSoundSourceManager::ET_returnSoundSource, this);
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
    OggDataStream* dataStream = controller->getDataStream();
    assert(dataStream != nullptr && "Invalid data stream");

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
            dataStream->setSampleOffset(0);
            int reqReadCount = SAMPLES_PER_READ - readSamplesCount;
            while (reqReadCount > 0) {
                int resReadCount = dataStream->readSamples(static_cast<int16_t*>(buffData) + readSamplesCount, reqReadCount);
                readSamplesCount += resReadCount;
                if(resReadCount < reqReadCount) {
                    dataStream->setSampleOffset(0);
                }
                reqReadCount = SAMPLES_PER_READ - readSamplesCount;
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
            state = ESourceState::WaitEnd;
            break;
        }
    }

    ALenum alError = alGetError();
    if(alError != AL_NO_ERROR) {
        LogWarning("[SoundSource::queueBuffers] Can't queue buffers. Error: %s", alGetString(alError));
    }
}

void SoundSource::resetALSourceParams() {
    alSourcef(sourceId, AL_GAIN, 1.f);
    alSourcef(sourceId, AL_PITCH, 1.f);
}