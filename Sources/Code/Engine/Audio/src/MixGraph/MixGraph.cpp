#include "MixGraph/MixGraph.hpp"
#include "MixGraph/SourceRampNode.hpp"
#include "Core/ETLogger.hpp"

#include <algorithm>

namespace {

const int MAX_SOURCES = 32;

} // namespace

MixGraph::MixGraph() :
    resampler(this) {
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
    auto outData = static_cast<float*>(out);

    int lowClipCount = 0;
    int highClipCount = 0;
    for(int i = 0; i < samples * channels; ++i) {
        if(out[i] > 1.f) {
            out[i] = 1.f;
            ++highClipCount;
        } else if(out[i] < -1.f) {
            out[i] = 1.f;
            ++lowClipCount;
        }
    }

    if(lowClipCount > 1) {
        LogWarning("[MixGraph::mix] Detected low-clipping: %d", lowClipCount);
    }
    if(highClipCount > 1) {
        LogWarning("[MixGraph::mix] Detected high-clipping: %d", highClipCount);
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
