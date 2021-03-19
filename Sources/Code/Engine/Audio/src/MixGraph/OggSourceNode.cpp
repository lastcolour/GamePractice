#include "MixGraph/OggSourceNode.hpp"
#include "MixGraph/CombineNode.hpp"
#include "MixGraph/MixConfig.hpp"
#include "MixGraph/MixGraph.hpp"
#include "Resampler.hpp"
#include "SoundProxy.hpp"

#include <cassert>

OggSourceNode::OggSourceNode(MixGraph* mixGraph) :
    MixNode(mixGraph),
    soundProxy(nullptr) {
}

OggSourceNode::~OggSourceNode() {
}

void OggSourceNode::additiveMixTo(float* out, int channels, int samples) {
   auto volume = soundProxy->readVolume();
   auto looped = soundProxy->readLooped();
   auto samplesOffset = soundProxy->readOffset();

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

    soundProxy->writeOffset(samplesOffset);

    if(mixState.isEnded && !looped) {
        resetState();
    }
}

bool OggSourceNode::setSound(SoundProxy& proxy) {
    assert(!soundProxy && "Invalid proxy");
    assert(!oggData.isOpened() && "Already opened another OGG data");

    soundProxy = &proxy;

    auto soundData = soundProxy->readData();
    assert(soundData && "Invalid stream sound data");

    if(!oggData.open(soundData->data)) {
        LogError("[OggSourceNode::attachToStream] Can't open OGG data");
        return false;
    }
    auto samplesOffset = soundProxy->readOffset();
    if(samplesOffset != 0) {
        oggData.setSampleOffset(samplesOffset);
    }
    return true;
}

void OggSourceNode::resetState() {
    oggData.close();
    soundProxy = nullptr;
    if(auto currParent = getParent()) {
        currParent->removeChild(this);
    }
}