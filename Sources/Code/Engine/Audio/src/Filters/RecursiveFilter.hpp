#ifndef __RECURSIVE_FILTER_HPP__
#define __RECURSIVE_FILTER_HPP__

struct RecursiveFilter {
public:

    RecursiveFilter();

    float apply(float val);

public:

    float a0;
    float a1;
    float a2;
    float b1;
    float b2;
    float c0;
    float d0;
    float x1;
    float x2;
    float y1;
    float y2;
};

RecursiveFilter CreateLowPass(float freq);

RecursiveFilter CreateHightPass(float freq);

RecursiveFilter CreatePeaking(float gain, float q, float freq);

RecursiveFilter CreateLowShelf(float gain, float freq);

RecursiveFilter CreateHighShelf(float gain, float freq);

#endif /* __RECURSIVE_FILTER_HPP__ */