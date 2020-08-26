#include "MixGraph/OggSourceNode.hpp"
#include "MixGraph/CombineNode.hpp"
#include "MixGraph/MixConfig.hpp"
#include "MixGraph/MixGraph.hpp"
#include "Resampler.hpp"
#include "SoundStream.hpp"
#include "Core/Buffer.hpp"
#include "OggDataStream.hpp"
#include "Core/ETLogger.hpp"

#include <cassert>

OggSourceNode::OggSourceNode() :
    soundStream(nullptr),
    samplesOffset(0),
    volume(1.f) {
}

OggSourceNode::~OggSourceNode() {
}

void OggSourceNode::openStream(SoundStream* stream) {
    assert(stream && "invalid stream");
    assert(!oggData.isOpened() && "already opened another stream");

    auto data = stream->getData();
    assert(data && "Invalid stream sound data");
    if(!oggData.open(data)) {
        LogError("[OggSourceNode::attachToStream] Can't open OGG data stream");
        setParent(nullptr);
        return;
    }
    samplesOffset = stream->getSamplesOffset();
    if(samplesOffset != 0) {
        oggData.setSampleOffset(samplesOffset);
    }
    volume = stream->getMixVolume();

    if(!stream->isEvent()) {
        soundStream = stream;
    }
}

void OggSourceNode::closeStream() {
    samplesOffset = 0;
    volume = 1.f;
    oggData.close();
    soundStream = nullptr;
}

void OggSourceNode::exclusiveMixTo(float* out, int channels, int samples) {
   bool looped = false;
   if(soundStream) {
       looped = soundStream->isMixLooped();
       volume = soundStream->getMixVolume();
   }

    auto mixState = getMixGraph()->getResampler().exclusiveResampleTo(out, channels, samples, looped, &oggData);
    samplesOffset += mixState.samplesRead;

    for(int i = 0; i < channels * samples; ++i) {
        out[i] *= volume;
    }
}

void OggSourceNode::additiveMixTo(float* out, int channels, int samples) {
   bool looped = false;
   if(soundStream) {
       looped = soundStream->isMixLooped();
       volume = soundStream->getMixVolume();
   }

    auto& tempBuffer = getMixGraph()->getTempBuffer();
    assert(tempBuffer.getSize() >= channels * samples * sizeof(float) && "Very small temp buffer");

    auto inData = static_cast<float*>(tempBuffer.getWriteData());
    std::fill_n(inData, channels * samples, 0.f);

    auto mixState = getMixGraph()->getResampler().exclusiveResampleTo(inData, channels, samples, looped, &oggData);
    samplesOffset += mixState.samplesRead;

    for(int i = 0; i < channels * samples; ++i) {
        out[i] += inData[i] * volume;
    }
}

unsigned int OggSourceNode::getSampleRate() const {
    return oggData.getSampleRate();
}

unsigned int OggSourceNode::getTotalSamples() const {
    return oggData.getTotalSamples();
}

SoundStream* OggSourceNode::getSoundStream() {
    return soundStream;
}

unsigned int OggSourceNode::getSamplesOffset() const {
    return samplesOffset;
}

void OggSourceNode::setSoundStream(SoundStream* newStream) {
    soundStream = newStream;
}