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
    assert(!dataStream && "Data streams should be detached at this moment");
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
    syncState.requestPaused(false);
    syncState.requestStopped(false);
    syncState.confirmStart();
}

void OboeSoundSource::stopStreaming() {
    syncState.requestStopped(true);
    while(!syncState.isEndConfirmed());
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

bool OboeSoundSource::isLooped() const {
    return syncState.getReadState().isLooped;
}

bool OboeSoundSource::isPaused() const {
    return syncState.getReadState().isPaused;
}

float OboeSoundSource::getGain() const {
    return syncState.getReadState().gain;
}

void OboeSoundSource::update() {
    if(dataStream && syncState.isEndConfirmed()) {
        stopStreaming();
    }
}

void OboeSoundSource::fillBuffer(float* outBuffer, int numFrames, int channels) {
    auto& currentState = syncState.getReadState();
    switch(streamState)
    {
    case EStreamState::Normal: {
        int readCount = dataStream->fillF32(outBuffer, numFrames, channels, currentState.isLooped);
        if(readCount < numFrames) {
            streamState = EStreamState::WaitEnd;
        }
        break;
    }
    case EStreamState::WaitEnd: {
        syncState.confirmEnd();
        break;
    }
    default:
        assert(false && "Invalid stream state");
        break;
    }
}

bool OboeSoundSource::queuryIsNeedStream() {
    if(syncState.isEndConfirmed()) {
        return false;
    }
    syncState.syncRead();
    auto& currentState = syncState.getReadState();
    if(currentState.isPaused) {
        return false;
    }
    if(currentState.isStopped) {
        syncState.confirmEnd();
        return false;
    }
    assert(dataStream && "Invalid data stream when trying to stream");
    return true;
}

int OboeSoundSource::getFrameRate() const {
    return dataStream->sampleRate;
}