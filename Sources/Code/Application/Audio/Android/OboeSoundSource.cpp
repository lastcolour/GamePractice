#include "Audio/Android/OboeSoundSource.hpp"
#include "Audio/OggDataStream.hpp"

#include <cassert>

OboeSoundSource::OboeSoundSource() :
    SoundSource(),
    dataStream(nullptr),
    looping(false),
    gain(1.f),
    state(ESourceState::Normal) {
}

OboeSoundSource::~OboeSoundSource() {
}

void OboeSoundSource::attachToDataStream(OggDataStream& newDataStream) {
    if(dataStream) {
        assert(false && "Invalind current data stream");
        return;
    }
    dataStream = &newDataStream;
    dataStream->setSampleOffset(0);
    looping = false;
    gain = 1.f;
}

void OboeSoundSource::stopStreaming() {
}

void OboeSoundSource::pauseStreaming() {
}

void OboeSoundSource::resumeStreaming() {
}

bool OboeSoundSource::isStreaming() const {
    return true;
}

void OboeSoundSource::setGain(float newGain) {
    gain = newGain;
}

void OboeSoundSource::setLoop(bool loopFlag) {
    looping = loopFlag;
}

bool OboeSoundSource::isLooped() const {
    return looping;
}

void OboeSoundSource::fillBuffer(float* outBuffer, int numFrames, int channels) {
    if(state == ESourceState::WaitEnd) {
        state = ESourceState::Ended;
        isEnded.store(true);
        return;
    }

    int readFrames = dataStream->readF32(outBuffer, numFrames, channels);
    if(looping) {
        int leftFrames = numFrames - readFrames;
        while(leftFrames > 0) {
            dataStream->setSampleOffset(0);
            readFrames += dataStream->readF32(outBuffer + readFrames, leftFrames, channels);
            leftFrames = numFrames - readFrames;
        }
    } else {
        if(readFrames < numFrames) {
           state = ESourceState::WaitEnd;
        }
    }
}

int OboeSoundSource::getFrameRate() const {
    assert(dataStream && "Invalid data stream");
    return dataStream->sampleRate;
}

float OboeSoundSource::getGain() const {
    return gain;
}