#ifndef __EQUALIZER_HPP__
#define __EQUALIZER_HPP__

#include "Filters/RecursiveFilter.hpp"
#include "Filters/EqualizerSetup.hpp"

struct Equalizer {
public:

    Equalizer();

    void setSetup(const EqualizerSetup& newSetup);
    void exclusiveMixTo(float* samples, int samplesCount, int channels);

private:

    EqualizerSetup setup;
    RecursiveFilter lfilters[5];
    RecursiveFilter rfilters[5];
    bool isUnit;
};

#endif /* __EQUALIZER_HPP__ */