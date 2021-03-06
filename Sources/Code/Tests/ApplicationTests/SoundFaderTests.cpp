#include "SoundFaderTests.hpp"
#include "Fader.hpp"

#include <vector>

TEST_F(SoundFaderTests, CheckFadeInMono) {
    const size_t SAMPLES_COUNT = 20;
    const size_t FADE_IN_SAMPLES_COUNT = 10;

    Fader fader;
    fader.setFadeIn(FADE_IN_SAMPLES_COUNT);

    std::vector<float> samples(SAMPLES_COUNT, 1.f);

    fader.exclusiveTransformFloat(&samples[0], 1, SAMPLES_COUNT);

    for(size_t i = 0; i < FADE_IN_SAMPLES_COUNT; ++i) {
        float resSample = samples[i];
        float expectedSample = i / static_cast<float>(FADE_IN_SAMPLES_COUNT);
        EXPECT_FLOAT_EQ(resSample, expectedSample);
    }

    for(size_t i = FADE_IN_SAMPLES_COUNT; i < SAMPLES_COUNT; ++i) {
        float resSample = samples[i];
        EXPECT_FLOAT_EQ(resSample, 1.f);
    }
}

TEST_F(SoundFaderTests, CheckFadeOutMono) {
    const size_t SAMPLES_COUNT = 20;
    const size_t FADE_OUT_SAMPLES_COUNT = 10;

    Fader fader;
    fader.setFadeOut(FADE_OUT_SAMPLES_COUNT);

    std::vector<float> samples(SAMPLES_COUNT, 1.f);

    fader.exclusiveTransformFloat(&samples[0], 1, SAMPLES_COUNT);

    for(size_t i = 0; i < FADE_OUT_SAMPLES_COUNT; ++i) {
        float resSample = samples[i];
        float expectedSample = 1.f - i / static_cast<float>(FADE_OUT_SAMPLES_COUNT);
        EXPECT_FLOAT_EQ(resSample, expectedSample);
    }

    for(size_t i = FADE_OUT_SAMPLES_COUNT; i < SAMPLES_COUNT; ++i) {
        float resSample = samples[i];
        EXPECT_FLOAT_EQ(resSample, 0.f);
    }
}

TEST_F(SoundFaderTests, CheckFadeInThenFadeOutMono) {
    const size_t SAMPLES_COUNT = 20;

    std::vector<float> samples(SAMPLES_COUNT, 1.f);

    Fader fader;
    {
        fader.setFadeIn(10);
        fader.exclusiveTransformFloat(&samples[0], 1, 5);
    }
    {
        fader.setFadeOut(15);
        fader.exclusiveTransformFloat(&samples[0] + 5, 1, SAMPLES_COUNT - 5);
    }

    int offset = 5;
    for(size_t i = 0; i < 5; ++i) {
        float resSample = samples[i];
        float expectedSample = i / static_cast<float>(10);
        EXPECT_FLOAT_EQ(resSample, expectedSample);
    }

    for(size_t i = offset; i < offset + 7; ++i) {
        float resSample = samples[i];
        float expectedSample = 1.f - (i - offset + 7) / static_cast<float>(15);
        EXPECT_FLOAT_EQ(resSample, expectedSample);
    }

    offset += 7;

    for(size_t i = offset; i < SAMPLES_COUNT; ++i) {
        float resSample = samples[i];
        EXPECT_FLOAT_EQ(resSample, 0.f);
    }
}

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

    int offset = 5;

    for(size_t i = offset; i < offset + 7; ++i) {
        float resSample = samples[i];
        float expectedSample =  (i - offset + 7) / static_cast<float>(15);
        EXPECT_FLOAT_EQ(resSample, expectedSample);
    }

    offset += 8;

    for(size_t i = offset; i < SAMPLES_COUNT; ++i) {
        float resSample = samples[i];
        EXPECT_FLOAT_EQ(resSample, 1.f);
    }
}