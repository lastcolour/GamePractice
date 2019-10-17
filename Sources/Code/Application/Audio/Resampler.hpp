#ifndef __RESAMPLER_HPP__
#define __RESAMPLER_HPP__

class Resampler {
public:

    Resampler();
    virtual ~Resampler();

    void convert(float* inArr, int inRate, float* outArr, int outRate);

private:

};

#endif /* __RESAMPLER_HPP__ */