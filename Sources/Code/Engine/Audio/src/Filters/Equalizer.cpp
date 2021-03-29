#include "Filters/Equalizer.hpp"
#include "AudioUtils.hpp"
#include "MixGraph/MixConfig.hpp"

namespace {

const float EQLowFreq = 60.f;
const float EQLowMidFreq = 250.f;
const float EQMidFreq = 1000.f;
const float EQHighMidFreq = 4000.f;
const float EQHighFreq = 16000.f;

const float Q_Factor = 0.6666f;

const int Lerp_Samples_Count = 256;

float convertToGain(float val) {
    if(val < 0) {
        return Math::Lerp(0.125f, 1.f, 1.f + val);
    } else {
        return Math::Lerp(1.f, 8.f, val);
    }
}

void applyStateMono(float* from, float* to, int samplesCount, EqualizerState& state) {
    if(state.isUnit) {
        for(int i = 0; i < samplesCount; ++i) {
            float val = from[i];
            to[i] = val * state.volume;
        }
    } else {
        for(int i = 0; i < samplesCount; ++i) {
            float val = from[i];
            for(int j = 0; j < 5; ++j) {
                val = state.lfilters[j].apply(val);
            }
            to[i] = val * state.volume;
        }
    }
}

void applyStateStereo(float* from, float* to, int samplesCount, EqualizerState& state) {
    if(state.isUnit) {
        for(int i = 0; i < samplesCount; ++i) {
            float lval = from[2 * i];
            float rval = from[2 * i + 1];
            to[2 * i] = lval * state.volume;
            to[2 * i + 1] = rval * state.volume;
        }
    } else {
        for(int i = 0; i < samplesCount; ++i) {
            float lval = from[2 * i];
            float rval = from[2 * i + 1];
            for(int j = 0; j < 5; ++j) {
                lval = state.lfilters[j].apply(lval);
                rval = state.rfilters[j].apply(rval);
            }
            to[2 * i] = lval * state.volume;
            to[2 * i + 1] = rval * state.volume;
        }
    }
}

} // namespace

Equalizer::Equalizer(const MixConfig* config) :
    mixConfig(config),
    performLerp(false) {

    setSetup(setup);
    performLerp = false;
}

void Equalizer::setSetup(const EqualizerSetup& newSetup) {
    prevState = currState;
    currState.isUnit = true;

    float lowGain = convertToGain(newSetup.low);
    currState.lfilters[0] = CreateLowShelf(lowGain, EQLowFreq / mixConfig->outSampleRate);
    if(abs(newSetup.low) > 0.01f) {
        currState.isUnit = false;
    }

    float lowMidGain = convertToGain(newSetup.lowMid);
    currState.lfilters[1] = CreatePeaking(lowMidGain, Q_Factor, EQLowMidFreq / mixConfig->outSampleRate);
    if(abs(newSetup.lowMid) > 0.01f) {
        currState.isUnit = false;
    }

    float midGain = convertToGain(newSetup.mid);
    currState.lfilters[2] = CreatePeaking(midGain, Q_Factor, EQMidFreq / mixConfig->outSampleRate);
    if(abs(newSetup.mid) > 0.01f) {
        currState.isUnit = false;
    }

    float midHighGain = convertToGain(newSetup.highMid);
    currState.lfilters[3] = CreatePeaking(midHighGain, Q_Factor, EQHighMidFreq / mixConfig->outSampleRate);
    if(abs(newSetup.highMid) > 0.01f) {
        currState.isUnit = false;
    }

    float highGain = convertToGain(newSetup.high);
    currState.lfilters[4] = CreateHighShelf(highGain, EQHighFreq / mixConfig->outSampleRate);
    if(abs(newSetup.high) > 0.01f) {
        currState.isUnit = false;
    }

    currState.rfilters[0] = currState.lfilters[0];
    currState.rfilters[1] = currState.lfilters[1];
    currState.rfilters[2] = currState.lfilters[2];
    currState.rfilters[3] = currState.lfilters[3];
    currState.rfilters[4] = currState.lfilters[4];
    currState.volume = newSetup.volume;

    setup = newSetup;
    performLerp = true;
}

void Equalizer::lerpMono(float* samples, int samplesCount) {
    std::vector<float> prevSamples;
    prevSamples.resize(samplesCount);

    applyStateMono(samples, &prevSamples[0], samplesCount, prevState);
    applyStateMono(samples, samples, samplesCount, currState);

    float t = 0.f;
    float step = 1.f / static_cast<float>(samplesCount);
    for(int i = 0; i < samplesCount; ++i) {
        samples[i] = Math::Lerp(prevSamples[i], samples[i], t);
        t += step;
    }
}

void Equalizer::lerpStereo(float* samples, int samplesCount) {
    std::vector<float> prevSamples;
    prevSamples.resize(2 * samplesCount);

    applyStateStereo(samples, &prevSamples[0], samplesCount, prevState);
    applyStateStereo(samples, samples, samplesCount, currState);

    float t = 0.f;
    float step = 1.f / static_cast<float>(samplesCount);
    for(int i = 0; i < samplesCount; ++i) {
        samples[2 * i] = Math::Lerp(prevSamples[2 * i], samples[2 * i], t);
        samples[2 * i + 1] = Math::Lerp(prevSamples[2 * i + 1], samples[2 * i + 1], t);
        t += step;
    }
}

void Equalizer::exclusiveTransform(float* samples, int channels, int samplesCount) {
    int startIdx = 0;
    if(performLerp) {
        int lerpSamples = std::min(samplesCount, Lerp_Samples_Count);
        performLerp = false;
        if(channels == 1) {
            lerpMono(samples, lerpSamples);
        } else {
            lerpStereo(samples, lerpSamples);
        }
        startIdx = lerpSamples;
    }
    if(channels == 1) {
        applyStateMono(samples + startIdx, samples + startIdx, samplesCount - startIdx, currState);
    } else {
        applyStateStereo(samples + 2 * startIdx, samples + 2 * startIdx, samplesCount - startIdx, currState);
    }
}