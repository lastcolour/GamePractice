#include "MixGraph/SourceNode.hpp"
#include "MixGraph/CombineNode.hpp"
#include "MixGraph/MixConfig.hpp"
#include "Resampler.hpp"
#include "SoundStream.hpp"
#include "Core/Buffer.hpp"

#include <cassert>

SourceNode::SourceNode() :
    soundStream(nullptr) {
}

SourceNode::~SourceNode() {
    if(soundStream) {
        soundStream->setMixNode(nullptr);
        soundStream = nullptr;
    }
}

void SourceNode::setStream(SoundStream* stream) {
    assert(stream && "invalid stream");
    assert(!soundStream && "already attached to another stream");
    soundStream = stream;
    soundStream->setMixNode(this);
}

void SourceNode::detachFromSource() {
    soundStream = nullptr;
    getParent()->removeChild(this);
}

void SourceNode::additiveMixTo(float* out, int channels, int samples) {
    if(!soundStream->isPlaying()) {
        detachFromSource();
        return;
    }

    auto& tempBuffer = getTempBuffer();
    assert(tempBuffer.getSize() >= channels * samples * sizeof(float) && "Very small temp buffer");

    auto inData = static_cast<float*>(tempBuffer.getWriteData());
    std::fill_n(inData, channels * samples, 0.f);

    getResampler().exclusiveResampleTo(inData, channels, samples, *soundStream);

    if(!soundStream) {
        return;
    }
    auto volume = soundStream->getVolume();
    for(int i = 0; i < channels * samples; ++i) {
        out[i] += inData[i] * volume;
    }
}