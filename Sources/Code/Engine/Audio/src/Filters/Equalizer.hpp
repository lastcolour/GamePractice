#ifndef __EQUALIZER_HPP__
#define __EQUALIZER_HPP__

#include "Filters/RecursiveFilter.hpp"
#include "Filters/EqualizerSetup.hpp"

struct MixConfig;

struct EqualizerState {
    RecursiveFilter lfilters[5];
    RecursiveFilter rfilters[5];
    float volume;
    bool isUnit;
};

struct Equalizer {
public:

    Equalizer(const MixConfig* config);

    void setSetup(const EqualizerSetup& newSetup);
    void exclusiveTransform(float* samples, int channels, int samplesCount);

private:

    void lerpMono(float* samples, int samplesCount);
    void lerpStereo(float* samples, int samplesCount);

private:

    const MixConfig* mixConfig;
    EqualizerSetup setup;
    EqualizerState prevState;
    EqualizerState currState;
    bool performLerp;
};

#endif /* __EQUALIZER_HPP__ */