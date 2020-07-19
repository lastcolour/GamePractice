#include "MixGraph/SourceNode.hpp"
#include "MixGraph/CombineNode.hpp"
#include "MixGraph/MixConfig.hpp"
#include "Resampler.hpp"
#include "SoundStream.hpp"
#include "Core/Buffer.hpp"
#include "OggDataStream.hpp"
#include "Core/ETLogger.hpp"

#include <cassert>

SourceNode::SourceNode() :
    soundStream(nullptr),
    samplesOffset(0),
    volume(1.f) {
}

SourceNode::~SourceNode() {
    if(soundStream) {
        soundStream->setMixNode(nullptr);
        soundStream = nullptr;
    }
}

void SourceNode::attachToStream(SoundStream* stream) {
    assert(stream && "invalid stream");
    assert(!oggData.isOpened() && "already opened another stream");

    auto& data = stream->getData();
    if(!oggData.open(data)) {
        LogError("[SourceNode::attachToStream] Can't open OGG data stream");
        getParent()->removeChild(this);
        return;
    }
    samplesOffset = stream->getSamplesOffset();
    oggData.setSampleOffset(samplesOffset);
    volume = stream->getMixVolume();

    if(!stream->isEvent()) {
        soundStream = stream;
        soundStream->setMixNode(this);
    }
}

void SourceNode::detachFromStream() {
    if(soundStream) {
        soundStream->setMixNode(nullptr);
        soundStream = nullptr;
    }
    samplesOffset = 0;
    volume = 1.f;
    oggData.close();
    getParent()->removeChild(this);
}

unsigned int SourceNode::getSamplesOffset() const {
    return samplesOffset;
}

void SourceNode::additiveMixTo(float* out, int channels, int samples) {
   bool looped = false;
   if(soundStream) {
       looped = soundStream->isMixLooped();
       volume = soundStream->getMixVolume();
   }

    auto& tempBuffer = getTempBuffer();
    assert(tempBuffer.getSize() >= channels * samples * sizeof(float) && "Very small temp buffer");

    auto inData = static_cast<float*>(tempBuffer.getWriteData());
    std::fill_n(inData, channels * samples, 0.f);

    auto mixState = getResampler().exclusiveResampleTo(inData, channels, samples, looped, &oggData);
    samplesOffset += mixState.samplesRead;

    for(int i = 0; i < channels * samples; ++i) {
        out[i] += inData[i] * volume;
    }

    if(mixState.isEnded) {
        detachFromStream();
    }
}