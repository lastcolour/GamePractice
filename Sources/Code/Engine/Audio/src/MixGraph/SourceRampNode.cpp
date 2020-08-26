#include "MixGraph/SourceRampNode.hpp"
#include "MixGraph/MixConfig.hpp"
#include "MixGraph/MixGraph.hpp"
#include "SoundStream.hpp"

#include <algorithm>
#include <cassert>

namespace {

const float FADE_IN_DURATION = 0.05f;
const float FADE_OUT_DURATION = 0.05f;

float additiveMixWithVolume(float* outData, float* inData, int channels, int startIdx, int endIdx, float startVolume, float volumeStep) {
    auto volume = startVolume;
    if(channels == 1) {
        for(int i = startIdx; i < endIdx; ++i) {
            outData[i] += inData[i] * volume;
            volume += volumeStep;
        }
    } else if (channels == 2) {
        for(int i = startIdx; i < endIdx; ++i) {
            outData[2 * i] += inData[2 * i] * volume;
            outData[2 * i + 1] += inData[2 * i + 1] * volume;
            volume += volumeStep;
        }
    } else {
        assert(false && "Too many channels");
    }
    return volume;
}

void additiveMix(float* outData, float* inData, int channels, int startIdx, int endIdx) {
    if(channels == 1) {
        for(int i = startIdx; i < endIdx; ++i) {
            outData[i] += inData[i];
        }
    } else if (channels == 2) {
        for(int i = startIdx; i < endIdx; ++i) {
            outData[2 * i] += inData[2 * i] ;
            outData[2 * i + 1] += inData[2 * i + 1];
        }
    } else {
        assert(false && "Too many channels");
    }
}

int getRemainingSamples(OggSourceNode& source, int outSampleRate) {
    int totalSamples = source.getTotalSamples() - source.getSamplesOffset();
    totalSamples = static_cast<int>(totalSamples * (source.getSampleRate() / static_cast<float>(outSampleRate)));
    return totalSamples;
}

} // namespace

SourceRampNode::SourceRampNode() :
    state(State::FadeIn),
    fadeintEnd(0),
    fadeoutStart(0),
    offset(0),
    maxSamplesToProcess(0),
    fraction(0.f),
    value(0.f) {
}

SourceRampNode::~SourceRampNode() {
    auto soundStream = oggSource.getSoundStream();
    if(soundStream) {
        soundStream->onDetachFromMixNode(0);
    }
    oggSource.closeStream();
}

void SourceRampNode::attachToStream(SoundStream* stream) {
    stream->onAttachToMixNode(this);
    oggSource.openStream(stream);
    oggSource.setMixGraph(getMixGraph());

    const auto& mixConfig = getMixGraph()->getMixConfig();

    state = State::FadeIn;
    offset = 0;
    value = 0.f;
    maxSamplesToProcess = getRemainingSamples(oggSource, mixConfig.outSampleRate);
    fadeintEnd = static_cast<int>(mixConfig.outSampleRate * FADE_IN_DURATION);
    fadeoutStart = maxSamplesToProcess - static_cast<int>(mixConfig.outSampleRate * FADE_OUT_DURATION);
    if(fadeoutStart < fadeintEnd) {
        auto mean = (fadeoutStart + fadeintEnd) / 2;
        fadeoutStart = mean;
        fadeintEnd = mean;
    }
    fraction = 1.f / static_cast<float>(fadeintEnd);
}

void SourceRampNode::detachFromStream() {
    const auto& mixConfig = getMixGraph()->getMixConfig();

    if(state != State::FadeOut) {
        int remainingSamples = getRemainingSamples(oggSource, mixConfig.outSampleRate);
        maxSamplesToProcess = std::min(remainingSamples, offset + static_cast<int>(mixConfig.outSampleRate * FADE_OUT_DURATION));

        fadeintEnd = offset;
        fadeoutStart = offset;
        state = State::FadeOut;
        fraction = -value / static_cast<float>(maxSamplesToProcess - offset);
    }

    auto stream = oggSource.getSoundStream();
    if(stream) {
        auto sourceSampleOffset = maxSamplesToProcess - offset;
        sourceSampleOffset = static_cast<int>(sourceSampleOffset * (oggSource.getSampleRate() / static_cast<float>(mixConfig.outSampleRate)));
        stream->onDetachFromMixNode(sourceSampleOffset);
        oggSource.setSoundStream(nullptr);
    }
}

void SourceRampNode::additiveMixTo(float* out, int channels, int samples) {
    auto& tempBuffer = getMixGraph()->getTempBuffer();
    assert(tempBuffer.getSize() >= channels * samples * sizeof(float) && "Very small temp buffer");

    auto inData = static_cast<float*>(tempBuffer.getWriteData());
    std::fill_n(inData, channels * samples, 0.f);

    oggSource.exclusiveMixTo(inData, channels, samples);

    int remaingSamples = samples;
    switch(state) {
        case State::FadeIn: {
            int prorcessSamples = std::min(samples, fadeintEnd - offset);
            int startIdx = 0;
            int endIdx = prorcessSamples;
            assert(endIdx <= samples);
            value = additiveMixWithVolume(out, inData, channels, startIdx, endIdx, value, fraction);
            assert((value <= 1.001f + fraction) && "Reach too high volume");
            offset += prorcessSamples;
            remaingSamples -= prorcessSamples;
            if(offset >= fadeintEnd) {
                state = State::Normal;
                value = std::min(value, 1.f);
                fraction = -value / static_cast<float>(maxSamplesToProcess - fadeoutStart);
            } else {
                break;
            }
            [[fallthrough]];
        }
        case State::Normal: {
            int prorcessSamples = std::min(offset + remaingSamples, fadeoutStart) - offset;
            int startIdx = samples - remaingSamples;
            int endIdx = startIdx + prorcessSamples;
            assert(endIdx <= samples);
            additiveMix(out, inData, channels, startIdx, endIdx);
            offset += prorcessSamples;
            remaingSamples -= prorcessSamples;
            if(offset >= fadeoutStart) {
                state = State::FadeOut;
            } else {
                break;
            }
            [[fallthrough]];
        }
        case State::FadeOut: {
            int startIdx = samples - remaingSamples;
            int endIdx = startIdx + std::min(remaingSamples, maxSamplesToProcess - offset);
            assert(endIdx <= samples);
            value = additiveMixWithVolume(out, inData, channels, startIdx, endIdx, value, fraction);
            assert((value >= -fraction - 0.001f) && "Reach too low volume");
            offset += remaingSamples;
            if(offset >= maxSamplesToProcess) {
                detachFromGraph();
            }
            break;
        }
        default: {
            assert(false && "Invalid state");
        }
    }
}

void SourceRampNode::detachFromGraph() {
    auto soundStream = oggSource.getSoundStream();
    if(soundStream) {
        soundStream->onDetachFromMixNode(0);
    }
    oggSource.closeStream();
    setParent(nullptr);
}