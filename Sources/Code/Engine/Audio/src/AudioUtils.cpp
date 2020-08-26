#include "AudioUtils.hpp"

#include <cmath>

namespace {

const float EXP_VOLUME_DYNAMIC_RANGE = 40.f;
const float EXP_VOLUME_POWER = powf(10.f, EXP_VOLUME_DYNAMIC_RANGE / 20.f);
const float EXP_VOLUME_A = 1.f / EXP_VOLUME_POWER;
const float EXP_VOLUME_B = logf(EXP_VOLUME_POWER);

} // namespace

namespace Audio {

float CovertToExpVolume(float value) {
    if(value <= 0.01) {
        return 0.f;
    }
    return EXP_VOLUME_A * expf(value * EXP_VOLUME_B);
}

} // namespace Audio