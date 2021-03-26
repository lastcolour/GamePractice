#include "MixGraph/OggSourceNode.hpp"
#include "MixGraph/CombineNode.hpp"
#include "MixGraph/MixConfig.hpp"
#include "MixGraph/MixGraph.hpp"
#include "Resampler.hpp"
#include "SoundProxy.hpp"

#include <cassert>

OggSourceNode::OggSourceNode(MixGraph* mixGraph) :
    MixNode(mixGraph) {
}

OggSourceNode::~OggSourceNode() {
}

void OggSourceNode::additiveMixTo(float* out, int channels, int samples) {
    auto volume = soundSource.getVolume();
    auto looped = soundSource.getLooped();

    auto& resampleBuffer = getMixGraph()->getResampleBuffer();
    assert(resampleBuffer.getSize() >= channels * samples * sizeof(float) && "Very small temp buffer");

    auto resamplerOutData = static_cast<float*>(resampleBuffer.getWriteData());
    std::fill_n(resamplerOutData, channels * samples, 0.f);

    auto mixState = getMixGraph()->getResampler().exclusiveResampleTo(resamplerOutData, channels, samples, looped, &oggData);

    fader.exclusiveTransformFloat(resamplerOutData, channels, samples);

    for(int i = 0; i < channels * samples; ++i) {
        out[i] += resamplerOutData[i] * volume;
    }

    auto samplesOffset = soundSource.getOffset();
    samplesOffset += mixState.samplesRead;
    samplesOffset %= oggData.getTotalSamples();
    soundSource.setOffset(samplesOffset);

    bool stopMixing = false;
    if(mixState.isEnded && !looped) {
        stopMixing = true;
    } else if(fader.isFadeOutDone()) {
        stopMixing = true;
    }
    if(stopMixing) {
        setSound(nullptr, false);
        if(auto currParent = getParent()) {
            currParent->removeChild(this);
        }
    }
}

bool OggSourceNode::setSound(SoundProxy* newSoundProxy, bool isEvent) {
    if(!soundSource.isNull()) {
        fader.reset();
        soundSource.reset();
        oggData.close();
    }
    if(!newSoundProxy) {
        return false;
    }

    soundSource = SoundSource(*newSoundProxy, isEvent);

    assert(!soundSource.getMixNode() && "Sound already has another mix node");

    if(oggData.open(soundSource.getData())) {
        soundSource.setMixNode(this);
    } else {
        LogError("[OggSourceNode::setSound] Can't open OGG data");
        soundSource.reset();
        return false;
    }
    auto samplesOffset = soundSource.getOffset();
    if(samplesOffset != 0) {
        oggData.setSampleOffset(samplesOffset);
    }
    return true;
}

void OggSourceNode::setResetOffsetOnStop(bool flag) {
    soundSource.setResetOffset(flag);
}

Fader& OggSourceNode::getFader() {
    return fader;
}