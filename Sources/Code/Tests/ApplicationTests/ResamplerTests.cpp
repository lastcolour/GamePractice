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
    req.inSamples = 4;
    req.outData = &outArr[0];

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

TEST_F(ResamplerTests, CheckConverPointStereo) {
    float inArr[] = {0.f, 0.f, 1.f, 1.f, 2.f, 2.f, 3.f, 3.f, 4.f, 4.f, 5.f, 5.f};
    float outArr[] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f};

    Resampler resampler;
    resampler.setOutChannels(2);
    resampler.setOutRate(5);

    ResampleRequest req;
    req.inData = &inArr[0];
    req.inRate = 6;
    req.inSamples = 6;
    req.outData = &outArr[0];

    resampler.convertPoint(req);

    EXPECT_FLOAT_EQ(outArr[0], 0.f);
    EXPECT_FLOAT_EQ(outArr[1], 0.f);

    EXPECT_FLOAT_EQ(outArr[2], 1.f);
    EXPECT_FLOAT_EQ(outArr[3], 1.f);

    EXPECT_FLOAT_EQ(outArr[4], 2.f);
    EXPECT_FLOAT_EQ(outArr[5], 2.f);

    EXPECT_FLOAT_EQ(outArr[6], 4.f);
    EXPECT_FLOAT_EQ(outArr[7], 4.f);

    EXPECT_FLOAT_EQ(outArr[8], 5.f);
    EXPECT_FLOAT_EQ(outArr[9], 5.f);
}

TEST_F(ResamplerTests, CheckConvertLinearMono) {
    float inArr[] = {0.f, 1.f};
    float outArr[] = {0.f};

    Resampler resampler;
    resampler.setOutChannels(1);
    resampler.setOutRate(1);

    ResampleRequest req;
    req.inData = &inArr[0];
    req.inRate = 2;
    req.inSamples = 2;
    req.outData = &outArr[0];

    resampler.convertLinear(req);

    EXPECT_FLOAT_EQ(outArr[0], 0.5f);
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
        EXPECT_EQ(resampler.getSamplesForConvert(44100, 960), 1044);
    }
    {
        Resampler resampler;
        resampler.setOutRate(44100);
        EXPECT_EQ(resampler.getSamplesForConvert(48000, 960), 882);
    }
}