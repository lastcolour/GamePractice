#include "MixGraph/MixGraph.hpp"
#include "MixGraph/MixNode.hpp"
#include "MixGraph/OggSourceNode.hpp"
#include "SoundStream.hpp"
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
    gameNode.setMixGraph(this);
    musicNode.setMixGraph(this);
    uiNode.setMixGraph(this);
    for(int i = 0; i < MAX_SOURCES; ++i) {
        auto source = new OggSourceNode();
        source->setMixGraph(this);
        sources.emplace_back(source);
    }

    lLowPass = CreateLowPass(20000.0 / 44100.f);
    rLowPass = lLowPass;

    return true;
}

void MixGraph::resizeBuffers(int channels, int samples) {
    auto minBufferSize = samples * channels * sizeof(float);
    if(resampleBuffer.getSize() < minBufferSize) {
        resampleBuffer.resize(minBufferSize);
    }
    if(combineBuffer.getSize() < minBufferSize) {
        combineBuffer.resize(minBufferSize);
    }
}

void MixGraph::mix(float* out, int channels, int samples) {
    std::fill_n(out, channels * samples, 0.f);

    resizeBuffers(channels, samples);

    gameNode.additiveMixTo(out, channels, samples);
    musicNode.additiveMixTo(out, channels, samples);
    uiNode.additiveMixTo(out, channels, samples);

    applyLowPass(out, channels, samples);

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
            break;
        }
    }
    return freeSource;
}

bool MixGraph::playSound(SoundStream* stream) {
    auto freeSourceNode = getFreeSource();
    if(!freeSourceNode) {
        LogWarning("[MixGraph::playSound] Can't find free source");
        return false;
    }
    CombineNode* combineNode = nullptr;
    auto soundGroup = stream->getGroup();
    switch(soundGroup) {
        case ESoundGroup::Game: {
            combineNode = &gameNode;
            break;
        }
        case ESoundGroup::Music: {
            combineNode = &musicNode;
            break;
        }
        case ESoundGroup::UI: {
            combineNode = &uiNode;
            break;
        }
        default: {
            assert(false && "Invalid sound group");
            return false;
        }
    }
    auto sourceNode = static_cast<OggSourceNode*>(freeSourceNode);
    if(sourceNode->attachToStream(stream)) {
        combineNode->addChild(sourceNode);
    }
    return true;
}

const MixConfig& MixGraph::getMixConfig() const {
    return config;
}

Resampler& MixGraph::getResampler() {
    return resampler;
}

Buffer& MixGraph::getResampleBuffer() {
    return resampleBuffer;
}

Buffer& MixGraph::getCombineBuffer() {
    return combineBuffer;
}

void MixGraph::setEqualizer(ESoundGroup soundGroup, const EqualizerSetup& eqSetup) {
    switch(soundGroup) {
        case ESoundGroup::Game: {
            gameNode.setEqualizerSetup(eqSetup);
            break;
        }
        case ESoundGroup::Music: {
            musicNode.setEqualizerSetup(eqSetup);
            break;
        }
        case ESoundGroup::UI: {
            uiNode.setEqualizerSetup(eqSetup);
            break;
        }
        default: {
            assert(false && "Invalid sound group");
        }
    }
}

void MixGraph::setMasterVolume(float newVolume) {
    assert(newVolume >= 0 && "Negative master volume");
    masterVolume = newVolume;
}

float MixGraph::getMasterVolume() const {
    return masterVolume;
}

void MixGraph::applyLowPass(float* out, int channels, int samples) {
    if(channels == 1) {
        for(int i = 0; i < samples; ++i) {
            out[i] = lLowPass.apply(out[i]);
        }
    } else if(channels == 2) {
        for(int i = 0; i < samples; ++i) {
            out[2 * i] = lLowPass.apply(out[2 * i]);
            out[2 * i + 1] = rLowPass.apply(out[2 * i + 1]);
        }
    }
}