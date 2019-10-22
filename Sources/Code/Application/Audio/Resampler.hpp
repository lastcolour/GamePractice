#ifndef __RESAMPLER_HPP__
#define __RESAMPLER_HPP__

struct ResampleRequest {
    float* outData;
    float* inData;
    int inRate;
    int inSamples;

    ResampleRequest() :
        outData(nullptr),
        inData(nullptr),
        inRate(0),
        inSamples(0) {}
};

class Resampler {
public:

    Resampler();
    virtual ~Resampler();

    void setOutRate(int newOutRate);
    void setOutChannels(int newOutChannels);
    int getSamplesForConvert(int inRate, int numSamples) const;

    int convertPoint(ResampleRequest& req);
    int convertLinear(ResampleRequest& req);
    int convertCubic(ResampleRequest& req);

private:

    void convertPointMono(ResampleRequest& req);
    void convertPointStereo(ResampleRequest& req);

    void convertLinearMono(ResampleRequest& req);
    void convertLinearStereo(ResampleRequest& req);

    void setupState(const ResampleRequest& req);
    void advance();
    int getIndex() const;
    float getFraction() const;

private:

    struct ConvertState {
        double frac;
        float fracOffset;
        int outSamples;

        ConvertState() : frac(0.0), fracOffset(0.f), outSamples(0) {}
    };

private:

    ConvertState state;
    int outRate;
    int outChannels;
};

#endif /* __RESAMPLER_HPP__ */