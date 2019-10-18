#include "ResamplerTests.hpp"
#include "Audio/Resampler.hpp"

TEST_F(ResamplerTests, CheckConvertPointTheSame) {
    float inArr[] = {0.f, 1.f, 2.f, 3.f};
    float outArr[] = {0.f, 0.f, 0.f, 0.f};
    
    Resampler resampler;
    resampler.setOutRate(4);

    ResampleRequest req;
    req.inData = &inArr[0];
    req.inRate = 4;
    req.outData = &outArr[0];
    req.inSamples = 1;

    int outSamples = resampler.convertPoint(req);

    EXPECT_EQ(outSamples, 4);
    for(int i = 0; i< outSamples; ++i) {
        EXPECT_FLOAT_EQ(outArr[i], inArr[i]);
    }
}

TEST_F(ResamplerTests, CheckConverPoint) {
    float inArr[] = {0.f, 1.f, 2.f, 3.f, 4.f, 5.f};
    float outArr[] = {0.f, 0.f, 0.f, 0.f};
    
    Resampler resampler;
    resampler.setOutRate(2);

    ResampleRequest req;
    req.inData = &inArr[0];
    req.inRate = 3;
    req.inSamples = 6;
    req.outData = &outArr[0];

    int resSamples = resampler.convertPoint(req);
    EXPECT_EQ(resSamples, 4);

    EXPECT_FLOAT_EQ(req.outData[0], 0.f);
    EXPECT_FLOAT_EQ(req.outData[1], 2.f);
    EXPECT_FLOAT_EQ(req.outData[2], 3.f);
    EXPECT_FLOAT_EQ(req.outData[3], 5.f);
}

TEST_F(ResamplerTests, CheckSamplesForConvert) {
    {
        Resampler resampler;
        resampler.setOutRate(2);
        EXPECT_EQ(resampler.getSamplesForConvert(3, 6), 4);

    }
    {
        Resampler resampler;
        resampler.setOutRate(4);
        EXPECT_EQ(resampler.getSamplesForConvert(4, 4), 4);
    }
    {
        Resampler resampler;
        resampler.setOutRate(48000);
        EXPECT_EQ(resampler.getSamplesForConvert(44100, 960), 0);
    }
    {
        Resampler resampler;
        resampler.setOutRate(44100);
        EXPECT_EQ(resampler.getSamplesForConvert(48000, 960), 0);
    }
}