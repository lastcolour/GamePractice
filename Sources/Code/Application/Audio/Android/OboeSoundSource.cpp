#include "Audio/Android/OboeSoundSource.hpp"
#include "Audio/OggDataStream.hpp"
#include "Audio/Sound.hpp"
#include "Audio/ETAudioInterfaces.hpp"

#include <cassert>

OboeSoundSource::OboeSoundSource() :
    SoundSource(),
    dataStream(nullptr),
    streamState(EStreamState::Normal) {
}

OboeSoundSource::~OboeSoundSource() {
    stopStreaming();
}

void OboeSoundSource::attachToDataStream(OggDataStream& newDataStream) {
    if(dataStream) {
        assert(false && "Invalind current data stream");
        return;
    }
    dataStream = &newDataStream;
    dataStream->setSampleOffset(0);
    streamState = EStreamState::Normal;

    syncState.requestGain(1.f);
    syncState.requestLooping(false);
    syncState.requestStopped(false);
    syncState.requestPaused(false);
}

void OboeSoundSource::stopStreaming() {
    syncState.requestStopped(true);
    while(isUseBuffer.load());
    if(dataStream->sound) {
        dataStream->sound->detachFromSource();
    }
    dataStream = nullptr;
    ET_SendEvent(&ETSoundSourceManager::ET_returnSoundSource, this);
}

void OboeSoundSource::pauseStreaming() {
    syncState.requestPaused(true);
}

void OboeSoundSource::resumeStreaming() {
    syncState.requestPaused(false);
}

void OboeSoundSource::setGain(float newGain) {
    syncState.requestGain(newGain);
}

void OboeSoundSource::setLoop(bool loopFlag) {
    syncState.requestLooping(loopFlag);
}

bool OboeSoundSource::isStreaming() const {
    return true;
}

bool OboeSoundSource::isLooped() const {
    return syncState.getWriteState()->isLooped;
}

float OboeSoundSource::getGain() const {
    return syncState.getReadState()->gain;
}

void OboeSoundSource::update() {
    if(dataStream == nullptr) {
        return;
    }
    if(isEnded.load()) {
        stopStreaming();
    }
}

void OboeSoundSource::fillBuffer(float* outBuffer, int numFrames, int channels) {
    syncState.sync();
    const OboeSourceState* currentState = syncState.getReadState();
    if(currentState->isPaused || currentState->isStopped) {
        return;
    }
    switch(streamState)
    {
    case EStreamState::Normal: {
        isUseBuffer.store(true);
        int readCount = dataStream->fillF32(outBuffer, numFrames, channels, currentState->isLooped);
        isUseBuffer.store(false);
        if(readCount < numFrames) {
            streamState = EStreamState::WaitEnd;
        }
        break;
    }
    case EStreamState::WaitEnd: {
        streamState = EStreamState::Ended;
        isEnded.store(true);
        break;
    }
    case EStreamState::Ended: {
        // free source && return it to audio system
        break;
    }
    default:
        break;
    }
}

int OboeSoundSource::getFrameRate() const {
    assert(dataStream && "Invalid data stream");
    return dataStream->sampleRate;
}