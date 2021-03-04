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

OggSourceNode::OggSourceNode(MixGraph* mixGraph) :
    MixNode(mixGraph),
    soundStream(nullptr),
    samplesOffset(0),
    volume(1.f) {
}

OggSourceNode::~OggSourceNode() {
}

void OggSourceNode::additiveMixTo(float* out, int channels, int samples) {
   bool looped = false;
   if(soundStream) {
       looped = soundStream->isMixLooped();
       volume = soundStream->getMixVolume();
   }

    auto& resampleBuffer = getMixGraph()->getResampleBuffer();
    assert(resampleBuffer.getSize() >= channels * samples * sizeof(float) && "Very small temp buffer");

    auto resamplerOutData = static_cast<float*>(resampleBuffer.getWriteData());
    std::fill_n(resamplerOutData, channels * samples, 0.f);

    auto mixState = getMixGraph()->getResampler().exclusiveResampleTo(resamplerOutData, channels, samples, looped, &oggData);
    samplesOffset += mixState.samplesRead;
    samplesOffset %= oggData.getTotalSamples();

    for(int i = 0; i < channels * samples; ++i) {
        out[i] += resamplerOutData[i] * volume;
    }

    if(mixState.isEnded && !looped) {
        detachFromStream();
    }
}

bool OggSourceNode::attachToStream(SoundStream* stream) {
    assert(stream && "Invalid stream");
    assert(!oggData.isOpened() && "Already opened another stream");

    auto data = stream->getData();
    assert(data && "Invalid stream sound data");
    if(!oggData.open(data)) {
        LogError("[OggSourceNode::attachToStream] Can't open OGG data stream");
        return false;
    }
    samplesOffset = stream->getSamplesOffset();
    if(samplesOffset != 0) {
        oggData.setSampleOffset(samplesOffset);
    }
    volume = stream->getMixVolume();
    if(!stream->isEvent()) {
        soundStream = stream;
    }
    stream->onAttachToMixNode(this);
    return true;
}

void OggSourceNode::detachFromStream() {
    if(soundStream) {
        soundStream->onDetachFromMixNode(samplesOffset);
        soundStream = nullptr;
    }
    samplesOffset = 0;
    volume = 1.f;
    oggData.close();
    if(auto currParent = getParent()) {
        currParent->removeChild(this);
    }
}