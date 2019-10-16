#include "Audio/Android/OboeMixer.hpp"
#include "Audio/Android/OboeSoundSource.hpp"
#include "ETApplicationInterfaces.hpp"

OboeMixer::OboeMixer() :
    mixBuffer(nullptr),
    mixBufferFrames(0),
    buffer(nullptr),
    format(oboe::AudioFormat::I16),
    channels(1),
    numFrames(0) {
}

OboeMixer::~OboeMixer() {
}

void OboeMixer::startMixing(int outChannels, oboe::AudioFormat outFormat, void* outBuffer, int outNumFrames) {
    buffer = outBuffer;
    format = outFormat;
    channels = outChannels;
    numFrames = outNumFrames;

    adjustMixBuffer();
    mixSilence();
}

void OboeMixer::addSource(OboeSoundSource& soundSource) {
    soundSource.fillBuffer(mixBuffer.get(), numFrames, channels);
}

void OboeMixer::endMixing() {
    if(format == oboe::AudioFormat::I16) {
        oboe::convertFloatToPcm16(mixBuffer.get(), static_cast<int16_t*>(buffer), numFrames * channels);
    } else {
        memcpy(buffer, mixBuffer.get(), numFrames * sizeof(float) * channels);
    }
}

void OboeMixer::adjustMixBuffer() {
    if(mixBufferFrames < numFrames) {
        mixBuffer.reset(new float[numFrames * channels]);
        if(!mixBuffer) {
            LogError("[OboeMixer::adjustMixBuffer] Can't resize mix buffer: %d -> %d", mixBufferFrames, numFrames * channels);
            return;
            mixBufferFrames = numFrames;
        }
    }
}

void OboeMixer::mixSilence() {
    memset(static_cast<void*>(mixBuffer.get()), 0, sizeof(float) * mixBufferFrames * 2);
    int outBufferSize = numFrames * channels;
    if(format == oboe::AudioFormat::Float) {
        outBufferSize *= sizeof(float);
    } else {
        outBufferSize *= sizeof(int16_t);
    }
    memset(buffer, 0, outBufferSize);

}