#include "Filters/Equalizer.hpp"
#include "Math/Primitivies.hpp"
#include "AudioUtils.hpp"

#include <cmath>

namespace {

const int SampleRate = 44100.f;

const float EQLowFreq = 60.f;
const float EQLowMidFreq = 250.f;
const float EQMidFreq = 1000.f;
const float EQHighMidFreq = 4000.f;
const float EQHighFreq = 16000.f;

const float Q_Factor = 0.6666f;

float convertToGain(float val) {
    if(val < 0) {
        return Math::Lerp(0.125f, 1.f, 1.f + val);
    } else {
        return Math::Lerp(1.f, 8.f, val);
    }
}

} // namespace

Equalizer::Equalizer() :
    isUnit(false) {
    setSetup(setup);
}

void Equalizer::setSetup(const EqualizerSetup& newSetup) {
    isUnit = true;

    float lowGain = convertToGain(newSetup.low);
    lfilters[0] = CreateLowShelf(lowGain, EQLowFreq / SampleRate);
    if(abs(newSetup.low) > 0.01f) {
        isUnit = false;
    }

    float lowMidGain = convertToGain(newSetup.lowMid);
    lfilters[1] = CreatePeaking(lowMidGain, Q_Factor, EQLowMidFreq / SampleRate);
    if(abs(newSetup.lowMid) > 0.01f) {
        isUnit = false;
    }

    float midGain = convertToGain(newSetup.mid);
    lfilters[2] = CreatePeaking(midGain, Q_Factor, EQMidFreq / SampleRate);
    if(abs(newSetup.mid) > 0.01f) {
        isUnit = false;
    }

    float midHighGain = convertToGain(newSetup.highMid);
    lfilters[3] = CreatePeaking(midHighGain, Q_Factor, EQHighMidFreq / SampleRate);
    if(abs(newSetup.highMid) > 0.01f) {
        isUnit = false;
    }

    float highGain = convertToGain(newSetup.high);
    lfilters[4] = CreateHighShelf(highGain, EQHighFreq / SampleRate);
    if(abs(newSetup.high) > 0.01f) {
        isUnit = false;
    }

    rfilters[0] = lfilters[0];
    rfilters[1] = lfilters[1];
    rfilters[2] = lfilters[2];
    rfilters[3] = lfilters[3];
    rfilters[4] = lfilters[4];

    setup = newSetup;
    setup.volume = Audio::CovertToExpVolume(setup.volume);
}

void Equalizer::exclusiveTransform(float* samples, int channels, int samplesCount) {
    if(!isUnit) {
        if(channels == 1) {
            for(int i = 0; i < samplesCount; ++i) {
                float val = samples[i];
                for(int j = 0; j < 5; ++j) {
                    val = lfilters[j].apply(val);
                }
                samples[i] = val;
            }
        } else if(channels == 2) {
            for(int i = 0; i < samplesCount; ++i) {
                float lval = samples[2 * i];
                float rval = samples[2 * i + 1];
                for(int j = 0; j < 5; ++j) {
                    lval = lfilters[j].apply(lval);
                    rval = rfilters[j].apply(rval);
                }
                samples[2 * i] = lval;
                samples[2 * i + 1] = rval;
            }
        }
    }
    for(int i = 0; i < samplesCount * channels; ++i) {
        samples[i] *= setup.volume;
    }
}