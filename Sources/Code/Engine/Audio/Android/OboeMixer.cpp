#include "Audio/Android/OboeMixer.hpp"
#include "Audio/Android/OboeSoundSource.hpp"
#include "ETApplicationInterfaces.hpp"

void OboeMixer::MixBuffer::updateSize(int newSize) {
    if(newSize > size) {
        size = newSize;
        data.reset(new float[size]);
    }
}

OboeMixer::OboeMixer() :
    buffer(nullptr),
    format(oboe::AudioFormat::I16),
    channels(1),
    numFrames(0) {

    resampler.setOutChannels(channels);
    resampler.setOutRate(44100);
}

OboeMixer::~OboeMixer() {
}

void OboeMixer::setOutRate(int deviceOutRate) {
    resampler.setOutRate(deviceOutRate);
}

void OboeMixer::startMixing(int outChannels, oboe::AudioFormat outFormat, void* outBuffer, int outNumFrames) {
    buffer = outBuffer;
    format = outFormat;
    channels = outChannels;
    numFrames = outNumFrames;

    resampler.setOutChannels(channels);
    mixBuffer.updateSize(numFrames * channels);
    mixSilence(mixBuffer);
}

void OboeMixer::mixSilence(MixBuffer& buffer) {
    auto& data = buffer.data;
    std::fill_n(data.get(), buffer.size, 0.f);
}

void OboeMixer::fillSourceBuffer(MixBuffer& sourceBuffer, OboeSoundSource& source, int readFramesCount) {
    auto& data = sourceBuffer.data;
    source.fillBuffer(data.get(), readFramesCount, channels);
}

void OboeMixer::resampleSourceBuffer(MixBuffer& sourceBuffer, OboeSoundSource& source, int inSamplesCount) {
    resampleBuffer.updateSize(sourceBuffer.size);

    ResampleRequest req;
    req.inData = sourceBuffer.data.get();
    req.inRate = source.getFrameRate();
    req.inSamples = inSamplesCount;
    req.outData = resampleBuffer.data.get();
    resampler.convertPoint(req);

    sourceBuffer.data.swap(resampleBuffer.data);
    std::swap(resampleBuffer.size, sourceBuffer.size);
}

void OboeMixer::postProcessSourceBuffer(MixBuffer& sourceBuffer, OboeSoundSource& source) {
    float gain = source.getGain();
    for(int i = 0; i < numFrames * channels; ++i) {
        sourceBuffer.data[i] *= gain;
    }
}

void OboeMixer::mergeSourceToMixBuffers(MixBuffer& sourceBuffer, MixBuffer& mixBuffer) {
    for(int i=0; i < numFrames * channels; ++i) {
        mixBuffer.data[i] += sourceBuffer.data[i];
    }
}

void OboeMixer::addSource(OboeSoundSource& soundSource) {
    if(!soundSource.queuryIsNeedStream()) {
        return;
    }
    int readSamplesCount = resampler.getSamplesForConvert(soundSource.getFrameRate(), numFrames);
    sourceBuffer.updateSize(readSamplesCount * channels);
    mixSilence(sourceBuffer);
    fillSourceBuffer(sourceBuffer, soundSource, readSamplesCount);
    if(readSamplesCount != numFrames) {
        resampleSourceBuffer(sourceBuffer, soundSource, readSamplesCount);
    }
    postProcessSourceBuffer(sourceBuffer, soundSource);
    mergeSourceToMixBuffers(sourceBuffer, mixBuffer);
}

void OboeMixer::endMixing() {
    auto& mixData = mixBuffer.data;
    if(format == oboe::AudioFormat::I16) {
        oboe::convertFloatToPcm16(mixData.get(), static_cast<int16_t*>(buffer), numFrames * channels);
    } else {
        memcpy(buffer, mixData.get(), numFrames * sizeof(float) * channels);
    }
}