#include "AudioResampleTests.hpp"
#include "MixGraph/LinearResampler.hpp"

TEST_F(AudioResampleTests, CheckResampleEqual) {
    LinearResampler resampler(10, 10);

    std::vector<float> inData;
    int samples = 10;
    for(int i = 0; i < samples; ++i) {
        inData.push_back(static_cast<float>(i));
    }

    std::vector<float> outData(inData.size(), 0.f);

    resampler.resampleMono(&inData[0], samples, &outData[0], samples);

    for(int i = 0; i < samples; ++i) {
        EXPECT_FLOAT_EQ(inData[i], outData[i]);
    }
}

TEST_F(AudioResampleTests, CheckResampleEqualTwoParts) {
    LinearResampler resampler(10, 10);

    std::vector<float> inData;
    int samples = 10;
    for(int i = 0; i < samples; ++i) {
        inData.push_back(static_cast<float>(i));
    }

    std::vector<float> outData(inData.size(), 0.f);

    int offset = samples / 2;
    resampler.resampleMono(&inData[0], samples / 2, &outData[0], samples / 2);
    resampler.resampleMono(&inData[0] + offset, samples / 2, &outData[0] + offset, samples / 2);

    for(int i = 0; i < samples; ++i) {
        EXPECT_FLOAT_EQ(inData[i], outData[i]);
    }
}

TEST_F(AudioResampleTests, CheckResampleIncreaseDecrease) {
    int inSamples = 10;
    int outSamples = 20;

    std::vector<float> inData;
    for(int i = 0; i < inSamples; ++i) {
        inData.push_back(i / static_cast<float>(inSamples));
    }

    LinearResampler incResampler(inSamples, outSamples);
    std::vector<float> outIncData(outSamples, 0.f);
    incResampler.resampleMono(&inData[0], inSamples, &outIncData[0], outSamples);

    LinearResampler decResampler(outSamples, inSamples);
    std::vector<float> outDecData(inSamples, 0.f);
    decResampler.resampleMono(&outIncData[0], outSamples, &outDecData[0], inSamples);

    for(int i = 0; i < inSamples; ++i) {
        EXPECT_FLOAT_EQ(outDecData[i], inData[i]) << i;
    }
}

TEST_F(AudioResampleTests, CheckResampleEqualStereo) {
    LinearResampler resampler(10, 10);

    std::vector<float> inData;
    int samples = 10;
    for(int i = 0; i < samples; ++i) {
        inData.push_back(static_cast<float>(i));
        inData.push_back(static_cast<float>(samples - i));
    }

    std::vector<float> outData(2 * inData.size(), 0.f);

    resampler.resampleStereo(&inData[0], samples, &outData[0], samples);

    for(int i = 0; i < samples; ++i) {
        EXPECT_FLOAT_EQ(inData[i], outData[i]);
    }
}