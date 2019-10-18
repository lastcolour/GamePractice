#include "Audio/Resampler.hpp"

#include <cmath>
#include <cassert>

Resampler::Resampler() :
    outRate(48000) {
}

Resampler::~Resampler() {
}

void Resampler::setOutRate(int newOutRate) {
    assert(newOutRate > 0 && "Negative new out rate");
    outRate = newOutRate;
}

int Resampler::getSamplesForConvert(int inRate, int numSamples) const {
    return inRate * numSamples / outRate;
}

int Resampler::convertPoint(ResampleRequest& req) {
    const int outSamples = req.inSamples * req.inRate / outRate;
    for(int i = 0; i < outSamples; ++i) {
        int nearestPt = req.inSamples * i / outRate;
        req.outData[i] = req.inData[nearestPt];
    }
    return outSamples;
}

int Resampler::convertLinear(ResampleRequest& req) {
    return 0;
}

int Resampler::convertCubic(ResampleRequest& req) {
    return 0;
}