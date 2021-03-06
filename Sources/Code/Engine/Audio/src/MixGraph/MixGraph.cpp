#include "MixGraph/MixGraph.hpp"
#include "MixGraph/MixNode.hpp"
#include "MixGraph/OggSourceNode.hpp"
#include "SoundStream.hpp"
#include "Core/ETLogger.hpp"
#include "AudioUtils.hpp"

#include <algorithm>
#include <cassert>

namespace {

const float LOW_PASS_FREQ = 20000.f;

} // namespace

MixGraph::MixGraph() :
    resampler(this),
    gameNode(this),
    musicNode(this),
    uiNode(this),
    masterVolume(1.f) {
}

MixGraph::~MixGraph() {
    sources.clear();
}

bool MixGraph::init(const MixConfig& newMixConfig) {
    if(!CheckMixConfig(newMixConfig)) {
        return false;
    }

    config = newMixConfig;
    for(int i = 0; i < config.maxSources; ++i) {
        auto source = new OggSourceNode(this);
        sources.emplace_back(source);
    }

    lLowPass = CreateLowPass(LOW_PASS_FREQ / static_cast<float>(config.outSampleRate));
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

void MixGraph::mixBufferAndConvert(float* out) {
    std::fill_n(out, config.channels * config.samplesPerBuffer, 0.f);

    resizeBuffers(config.channels, config.samplesPerBuffer);

    gameNode.additiveMixTo(out, config.channels, config.samplesPerBuffer);
    musicNode.additiveMixTo(out, config.channels, config.samplesPerBuffer);
    uiNode.additiveMixTo(out, config.channels, config.samplesPerBuffer);

    applyLowPass(out, config.channels, config.samplesPerBuffer);

    int lowClipCount = 0;
    int highClipCount = 0;
    for(int i = 0; i < config.channels * config.samplesPerBuffer; ++i) {
        out[i] *= masterVolume;
        if(out[i] > 1.f) {
            out[i] = 1.f;
            ++highClipCount;
        } else if(out[i] < -1.f) {
            out[i] = -1.f;
            ++lowClipCount;
        }
    }

    if(lowClipCount > 1 || highClipCount > 1) {
        LogWarning("[MixGraph::mix] Detected clipping: LOW = %d, HIGHT = %d", lowClipCount, highClipCount);
    }

    Audio::ConverFloatToInt16(out, reinterpret_cast<int16_t*>(out),
        config.channels * config.samplesPerBuffer);
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