#include "MixGraph/MixGraph.hpp"
#include "MixGraph/MixNode.hpp"
#include "MixGraph/OggSourceNode.hpp"
#include "AudioUtils.hpp"
#include "SoundProxy.hpp"
#include "Fader.hpp"

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
    if(!CheckAndPrintMixConfig(newMixConfig)) {
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

    Audio::ConverFloatsToInt16(out, reinterpret_cast<int16_t*>(out),
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

CombineNode* MixGraph::getCombineNode(ESoundGroup soundGroup) {
    CombineNode* combineNode = nullptr;
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
        }
    }
    return combineNode;
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
    auto combineNode = getCombineNode(soundGroup);
    if(!combineNode) {
        return;
    }
    combineNode->setEqualizerSetup(eqSetup);
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

bool MixGraph::startSound(SoundProxy& soundProxy, float duration, bool isEvent) {
    if(soundProxy.getMixNode()) {
        assert(false && "Invalid mix node");
        return false;
    }
    auto freeSourceNode = getFreeSource();
    if(!freeSourceNode) {
        LogWarning("[MixGraph::startSound] Can't find free source");
        return false;
    }
    auto soundGroup = soundProxy.readGroup();
    auto combineNode = getCombineNode(soundGroup);
    if(!combineNode) {
        return false;
    }

    auto sourceNode = static_cast<OggSourceNode*>(freeSourceNode);
    if(duration > 0.f) {
        auto& fader = sourceNode->getFader();
        int samples = static_cast<int>(config.outSampleRate * duration);
        fader.setFadeIn(samples);
    }
    if(sourceNode->setSound(&soundProxy, isEvent)) {
        combineNode->addChild(sourceNode);
        return true;
    }
    return false;
}

void MixGraph::stopSound(SoundProxy& soundProxy, float duration, bool resetOffset) {
    auto mixNode = soundProxy.getMixNode();
    if(!mixNode) {
        assert(false && "Invalid mix node");
        return;
    }
    auto oggSource = static_cast<OggSourceNode*>(mixNode);
    oggSource->setResetOffsetOnStop(resetOffset);
    if(duration < 0.f) {
        oggSource->setSound(nullptr, false);
        oggSource->getParent()->removeChild(oggSource);
    } else {
        auto& fader = oggSource->getFader();
        int samples = static_cast<int>(config.outSampleRate * duration);
        fader.setFadeOut(samples);
    }
}