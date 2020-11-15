#include "MixGraph/MixGraph.hpp"
#include "MixGraph/SourceRampNode.hpp"
#include "Core/ETLogger.hpp"

#include <algorithm>
#include <cassert>

namespace {

const int MAX_SOURCES = 32;

} // namespace

MixGraph::MixGraph() :
    resampler(this),
    masterVolume(1.f) {
}

MixGraph::~MixGraph() {
    sources.clear();
}

bool MixGraph::init() {
    rootCombine.setMixGraph(this);
    for(int i = 0; i < MAX_SOURCES; ++i) {
        auto source = new SourceRampNode();
        source->setMixGraph(this);
        sources.emplace_back(source);
    }
    return true;
}

void MixGraph::mix(float* out, int channels, int samples) {
    std::fill_n(out, channels * samples, 0.f);

    auto minBufferSize = samples * channels * sizeof(float);
    if(buffer.getSize() < minBufferSize) {
        buffer.resize(minBufferSize);
    }

    rootCombine.additiveMixTo(out, channels, samples);
    equalizer.exclusiveMixTo(out, samples, channels);

    int lowClipCount = 0;
    int highClipCount = 0;
    for(int i = 0; i < samples * channels; ++i) {
        out[i] *= masterVolume;
        if(out[i] > 1.f) {
            out[i] = 1.f;
            ++highClipCount;
        } else if(out[i] < -1.f) {
            out[i] = 1.f;
            ++lowClipCount;
        }
    }

    if(lowClipCount > 1 || highClipCount > 1) {
        LogWarning("[MixGraph::mix] Detected clipping: LOW = %d, HIGHT = %d", lowClipCount, highClipCount);
    }
}

MixNode* MixGraph::getFreeSource() {
    MixNode* freeSource = nullptr;
    for(auto& source : sources) {
        if(!source->getParent()) {
            freeSource = source.get();
        }
    }
    return freeSource;
}

bool MixGraph::playSound(SoundStream* stream) {
    auto mixNode = getFreeSource();
    if(!mixNode) {
        LogWarning("[MixGraph::playSound] Can't find free source");
        return false;
    }
    auto sourceNode = static_cast<SourceRampNode*>(mixNode);
    sourceNode->attachToStream(stream);
    rootCombine.addChild(mixNode);
    return true;
}

const MixConfig& MixGraph::getMixConfig() const {
    return config;
}

Resampler& MixGraph::getResampler() {
    return resampler;
}

Buffer& MixGraph::getTempBuffer() {
    return buffer;
}

void MixGraph::setEqualizer(ESoundGroup soundGroup, const EqualizerSetup& eqSetup) {
    equalizer.setSetup(eqSetup);
}

void MixGraph::setMasterVolume(float newVolume) {
    assert(newVolume >= 0 && "Negative master volume");
    masterVolume = newVolume;
}

float MixGraph::getMasterVolume() const {
    return masterVolume;
}