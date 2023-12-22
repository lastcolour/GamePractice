#include "SoundFaderTests.hpp"
#include "Fader.hpp"

TEST_F(SoundFaderTests, CheckFadingCurveChangeMono) {
    const size_t SAMPLES_COUNT = 20;

    std::vector<float> samples(SAMPLES_COUNT, 1.f);

    Fader fader;
    fader.setFadeIn(10);
    fader.exclusiveTransformFloat(&samples[0], 1, 5);

    fader.setFadeIn(15);
    fader.exclusiveTransformFloat(&samples[0] + 5, 1, 15);

    for(size_t i = 0; i < 5; ++i) {
        float resSample = samples[i];
        float expectedSample = i / static_cast<float>(10);
        EXPECT_FLOAT_EQ(resSample, expectedSample);
    }

    size_t offset = 5;

    for(size_t i = offset; i < offset + 7u; ++i) {
        float resSample = samples[i];
        float expectedSample = (i - offset + 7) / static_cast<float>(15);
        EXPECT_FLOAT_EQ(resSample, expectedSample);
    }

    offset += 8;

    for(size_t i = offset; i < SAMPLES_COUNT; ++i) {
        float resSample = samples[i];
        EXPECT_FLOAT_EQ(resSample, 1.f);
    }
}

TEST_F(SoundFaderTests, CheckFadeOutFadeInInt16Mono) {
    const size_t SAMPLES_COUNT = 30;
    std::vector<int16_t> samples;
    for(size_t i = 0u; i < SAMPLES_COUNT; ++i) {
        samples.push_back(std::numeric_limits<int16_t>::max());
    }

    int channels = 1;

    Fader fader;
    fader.setFadeOut(10);
    fader.exclusiveTransformInt16(&samples[0], channels, 15);

    fader.setFadeIn(10);
    fader.exclusiveTransformInt16(&samples[0] + 15 * channels, channels, 15);

    for(int i = 0; i < 10; ++i) {
        int lval = std::numeric_limits<int16_t>::max() * (10 - i) / 10;
        EXPECT_EQ(lval, samples[i]);
    }

    for(int i = 10; i < 15; ++i) {
        int lval = 0;
        EXPECT_EQ(lval, samples[i]);
    }

    for(int i = 15; i < 25; ++i) {
        int lval = std::numeric_limits<int16_t>::max() * (i - 15) / 10;
        EXPECT_EQ(lval, samples[i]);
    }

    for(int i = 25; i < 30; ++i) {
        int lval = std::numeric_limits<int16_t>::max();
        EXPECT_EQ(lval, samples[i]);
    }
}

TEST_F(SoundFaderTests, CheckFadeOutFadeFloatMono) {
    const size_t SAMPLES_COUNT = 30;
    std::vector<float> samples;
    for(size_t i = 0; i < SAMPLES_COUNT; ++i) {
        samples.push_back(1.f);
    }

    int channels = 1;

    Fader fader;
    fader.setFadeOut(10);
    fader.exclusiveTransformFloat(&samples[0], channels, 15);

    fader.setFadeIn(10);
    fader.exclusiveTransformFloat(&samples[0] + 15 * channels, channels, 15);

    for(int i = 0; i < 10; ++i) {
        float lval = 1.f * (10 - i) / 10;
        EXPECT_FLOAT_EQ(lval, samples[i]);
    }

    for(int i = 10; i < 15; ++i) {
        float lval = 0.f;
        EXPECT_FLOAT_EQ(lval, samples[i]);
    }

    for(int i = 15; i < 25; ++i) {
        float lval = 1.f * (i - 15) / 10;
        EXPECT_FLOAT_EQ(lval, samples[i]);
    }

    for(int i = 25; i < 30; ++i) {
        float lval = 1.f;
        EXPECT_FLOAT_EQ(lval, samples[i]);
    }
}

TEST_F(SoundFaderTests, CheckFadeOutFadeInInt16Stereo) {
    const size_t SAMPLES_COUNT = 30;
    std::vector<int16_t> samples;
    for(size_t i = 0u; i < SAMPLES_COUNT; ++i) {
        samples.push_back(std::numeric_limits<int16_t>::max());
        samples.push_back(std::numeric_limits<int16_t>::min());
    }

    int channels = 2;

    Fader fader;
    fader.setFadeOut(10);
    fader.exclusiveTransformInt16(&samples[0], channels, 15);

    fader.setFadeIn(10);
    fader.exclusiveTransformInt16(&samples[0] + 15 * channels, channels, 15);

    for(int i = 0; i < 10; ++i) {
        int lval = std::numeric_limits<int16_t>::max() * (10 - i) / 10;
        int rval = std::numeric_limits<int16_t>::min() * (10 - i) / 10;
        EXPECT_EQ(lval, samples[2 * i]);
        EXPECT_EQ(rval, samples[2 * i + 1]);
    }

    for(int i = 10; i < 15; ++i) {
        int lval = 0;
        int rval = 0;
        EXPECT_EQ(lval, samples[2 * i]);
        EXPECT_EQ(rval, samples[2 * i + 1]);
    }

    for(int i = 15; i < 25; ++i) {
        int lval = std::numeric_limits<int16_t>::max() * (i - 15) / 10;
        int rval = std::numeric_limits<int16_t>::min() * (i - 15) / 10;
        EXPECT_EQ(lval, samples[2 * i]);
        EXPECT_EQ(rval, samples[2 * i + 1]);
    }

    for(int i = 25; i < 30; ++i) {
        int lval = std::numeric_limits<int16_t>::max();
        int rval = std::numeric_limits<int16_t>::min();
        EXPECT_EQ(lval, samples[2 * i]);
        EXPECT_EQ(rval, samples[2 * i + 1]);
    }
}

TEST_F(SoundFaderTests, CheckFadeOutFadeFloatStereo) {
    const size_t SAMPLES_COUNT = 30;
    std::vector<float> samples;
    for(size_t i = 0u; i < SAMPLES_COUNT; ++i) {
        samples.push_back(1.f);
        samples.push_back(-1.f);
    }

    int channels = 2;

    Fader fader;
    fader.setFadeOut(10);
    fader.exclusiveTransformFloat(&samples[0], channels, 15);

    fader.setFadeIn(10);
    fader.exclusiveTransformFloat(&samples[0] + 15 * channels, channels, 15);

    for(int i = 0; i < 10; ++i) {
        float lval = 1.f * (10 - i) / 10;
        float rval = -1.f * (10 - i) / 10;
        EXPECT_FLOAT_EQ(lval, samples[2 * i]);
        EXPECT_FLOAT_EQ(rval, samples[2 * i + 1]);
    }

    for(int i = 10; i < 15; ++i) {
        float lval = 0.f;
        float rval = 0.f;
        EXPECT_FLOAT_EQ(lval, samples[2 * i]);
        EXPECT_FLOAT_EQ(rval, samples[2 * i + 1]);
    }

    for(int i = 15; i < 25; ++i) {
        float lval = 1.f * (i - 15) / 10;
        float rval = -1.f * (i - 15) / 10;
        EXPECT_FLOAT_EQ(lval, samples[2 * i]);
        EXPECT_FLOAT_EQ(rval, samples[2 * i + 1]);
    }

    for(int i = 25; i < 30; ++i) {
        float lval = 1.f;
        float rval = -1.f;
        EXPECT_FLOAT_EQ(lval, samples[2 * i]);
        EXPECT_FLOAT_EQ(rval, samples[2 * i + 1]);
    }
}