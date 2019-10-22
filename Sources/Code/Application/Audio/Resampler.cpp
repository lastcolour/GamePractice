#include "Audio/Resampler.hpp"

#include <cmath>
#include <cassert>

namespace {

float LinearLerp(float a, float b, float alpha) {
    return a + (b - a) * alpha;
}

} // namespace

Resampler::Resampler() :
    outRate(48000),
    outChannels(1) {
}

Resampler::~Resampler() {
}

void Resampler::setOutRate(int newOutRate) {
    assert(newOutRate > 0 && "Negative new out rate");
    outRate = newOutRate;
}

void Resampler::setOutChannels(int newOutChannels) {
    assert((outChannels == 1 || outChannels == 2) && "Invalid number of channles");
    outChannels = newOutChannels;
}

int Resampler::getSamplesForConvert(int inRate, int numSamples) const {
    return outRate * numSamples / inRate;
}

void Resampler::setupState(const ResampleRequest& req) {
    state.outSamples = getSamplesForConvert(req.inRate, req.inSamples);
    state.frac = req.inRate / static_cast<float>(outRate);
    state.fracOffset = 0;
}

int Resampler::getIndex() const {
    return static_cast<int>(state.fracOffset + 0.5);
}

float Resampler::getFraction() const {
    double tempVar;
    return static_cast<float>(std::modf(state.fracOffset, &tempVar));
}

void Resampler::advance() {
    state.fracOffset += state.frac;
}

void Resampler::convertPointMono(ResampleRequest& req) {
    for(int i = 0; i < state.outSamples; ++i) {
        auto n = getIndex();
        req.outData[i] = req.inData[n];
        advance();
    }
}

void Resampler::convertPointStereo(ResampleRequest& req) {
    for(int i = 0; i < state.outSamples; ++i) {
        auto idx = 2 * getIndex();
        req.outData[2 * i] = req.inData[idx];
        req.outData[2 * i + 1] = req.inData[idx + 1];
        advance();
    }
}

int Resampler::convertPoint(ResampleRequest& req) {
    setupState(req);
    if(outChannels == 1) {
        convertPointMono(req);
    } else {
        convertPointStereo(req);
    }
    return state.outSamples;
}

void Resampler::convertLinearMono(ResampleRequest& req) {
    for(int i = 0; i < state.outSamples; ++i) {
        auto idx = getIndex();
        auto alpha = getFraction();
        req.outData[i] = LinearLerp(req.outData[idx], req.outData[idx + 1], alpha);
        advance();
    }
}

void Resampler::convertLinearStereo(ResampleRequest& req) {
    for(int i = 0; i < state.outSamples; ++i) {
        auto idx = 2 * getIndex();
        auto alpha = getFraction();
        req.outData[2 * i] = LinearLerp(req.outData[idx], req.outData[idx + 2], alpha);
        req.outData[2 * i + 1] = LinearLerp(req.outData[idx], req.outData[idx + 2], alpha);
        advance();
    }
}

int Resampler::convertLinear(ResampleRequest& req) {
    setupState(req);
    if(outChannels == 1) {
        convertLinearMono(req);
    } else {
        convertLinearStereo(req);
    }
    return state.outSamples;
}

int Resampler::convertCubic(ResampleRequest& req) {
    return 0;
}