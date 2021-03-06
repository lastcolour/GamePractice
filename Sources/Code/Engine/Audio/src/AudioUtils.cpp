#include "AudioUtils.hpp"

#include <cmath>
#include <algorithm>

namespace {

const float EXP_VOLUME_DYNAMIC_RANGE = 40.f;
const float EXP_VOLUME_POWER = pow(10.f, EXP_VOLUME_DYNAMIC_RANGE / 20.f);
const float EXP_VOLUME_A = 1.f / EXP_VOLUME_POWER;
const float EXP_VOLUME_B = log(EXP_VOLUME_POWER);

} // namespace

namespace Audio {

float ConvertToExpVolume(float value) {
    if(value <= 0.001) {
        return 0.f;
    }
    return EXP_VOLUME_A * exp(value * EXP_VOLUME_B);
}

void ConverFloatsToInt16(float* from, int16_t* to, int samples) {
    for(int i = 0; i < samples; ++i) {
        float fVal = from[i];
        fVal += 1.0;
        fVal *= 32768.0f;
        auto iVal = static_cast<int32_t>(fVal);
        iVal = std::min(65536, std::max(0, iVal));
        iVal -= 32768;
        to[i] = static_cast<int16_t>(iVal);
    }
}

} // namespace Audio