#include "Audio/Android/OboeSoundSource.hpp"
#include "Audio/OggDataStream.hpp"

#include <cassert>

OboeSoundSource::OboeSoundSource() :
    SoundSource(),
    controller(nullptr),
    looped(false),
    gain(1.f),
    state(ESourceState::Normal) {
}

OboeSoundSource::~OboeSoundSource() {
}

void OboeSoundSource::attachToController(SoundSourceController& newController) {
    if(controller) {
        assert(false && "Invalind current stream controller");
        return;
    }
    controller = &newController;
    controller->getDataStream()->setSampleOffset(0);
    looped = false;
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
    looped = loopFlag;
}

bool OboeSoundSource::isLooped() const {
    return looped;
}

void OboeSoundSource::fillBuffer(float* outBuffer, int numFrames, int channels) {
    if(state == ESourceState::WaitEnd) {
        state = ESourceState::Ended;
        isEnded.store(true);
        return;
    }

    auto dataStream = controller->getDataStream();
    int readFrames = dataStream->readF32(outBuffer, numFrames, channels);
    if(looped) {
        int leftFrames = numFrames - readFrames;
        while(leftFrames > 0) {
            dataStream->setSampleOffset(0);
            int offset = leftFrames * 2;
            leftFrames -= dataStream->readF32(outBuffer + offset, leftFrames, channels);
        }
    } else {
        if(readFrames < numFrames) {
           state = ESourceState::WaitEnd;
        }
    }

    // postprocess
}
