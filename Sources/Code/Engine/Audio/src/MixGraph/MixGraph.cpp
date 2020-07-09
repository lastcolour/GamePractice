#include "MixGraph/MixGraph.hpp"
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
    root.setMixGraph(this);
    for(int i = 0; i < MAX_SOURCES; ++i) {
        auto source = new SourceNode();
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

    root.additiveMixTo(out, channels, samples);
    auto outData = static_cast<float*>(out);

    for(int i = 0; i < samples * channels; ++i) {
        out[i] = std::min(1.f, std::max(out[i], -1.f));
    } 
}

SourceNode* MixGraph::getFreeSource() {
    SourceNode* freeSource = nullptr;
    for(auto& source : sources) {
        if(!source->getParent()) {
            freeSource = source.get();
        }
    }
    return freeSource;
}

bool MixGraph::playSound(SoundStream* stream) {
    auto source = getFreeSource();
    if(!source) {
        LogWarning("[MixGraph::playSound] Can't find free source");
        return false;
    }
    source->setStream(stream);
    root.addChild(source);
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
