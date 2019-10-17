#include "ResamplerTests.hpp"
#include "Audio/Resampler.hpp"

TEST_F(ResamplerTests, CheckPrimitiveCase) {
    float inArr[] = {0.f, 1.f, 0.f};
    int inRate = 3;

    float outArr[] = {-1.f, -1.f};
    int outRate = 2;

    Resampler resampler;
    resampler.convert(inArr, inRate, outArr, outRate);
}