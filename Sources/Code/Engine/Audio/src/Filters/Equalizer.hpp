#ifndef __EQUALIZER_HPP__
#define __EQUALIZER_HPP__

#include "Filters/RecursiveFilter.hpp"
#include "Filters/EqualizerSetup.hpp"

struct Equalizer {
public:

    Equalizer();

    void setSetup(const EqualizerSetup& newSetup);
    void exclusiveTransform(float* samples, int channels, int samplesCount);

private:

    EqualizerSetup setup;
    RecursiveFilter lfilters[5];
    RecursiveFilter rfilters[5];
    bool isUnit;
};

#endif /* __EQUALIZER_HPP__ */