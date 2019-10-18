#ifndef __RESAMPLER_HPP__
#define __RESAMPLER_HPP__

struct ResampleRequest {
    float* inData;
    int inRate;
    int inSamples;
    float* outData;
};

class Resampler {
public:

    Resampler();
    virtual ~Resampler();

    void setOutRate(int newOutRate);
    int getSamplesForConvert(int inRate, int numSamples) const;

    int convertPoint(ResampleRequest& req);
    int convertLinear(ResampleRequest& req);
    int convertCubic(ResampleRequest& req);

private:

    int outRate;
};

#endif /* __RESAMPLER_HPP__ */