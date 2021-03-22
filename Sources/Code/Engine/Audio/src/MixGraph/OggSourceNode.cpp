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
   auto samplesOffset = soundProxy->getOffset();

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

    soundProxy->setOffset(samplesOffset);

    if(mixState.isEnded && !looped) {
        setSound(nullptr);
        if(auto currParent = getParent()) {
            currParent->removeChild(this);
        }
    }
}

bool OggSourceNode::setSound(SoundProxy* proxy) {
    if(soundProxy == proxy) {
        return false;
    }
    if(soundProxy) {
        oggData.close();
        soundProxy->setMixNode(nullptr);
        soundProxy = nullptr;
    }

    soundProxy = proxy;
    if(!soundProxy) {
        return false;
    }

    assert(!soundProxy->getMixNode() && "Sound already has another mix node");
    soundProxy->setMixNode(this);

    if(!oggData.open(soundProxy->readData())) {
        LogError("[OggSourceNode::attachToStream] Can't open OGG data");
        return false;
    }
    auto samplesOffset = soundProxy->getOffset();
    if(samplesOffset != 0) {
        oggData.setSampleOffset(samplesOffset);
    }
    return true;
}