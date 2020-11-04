#include "AudioUtils.hpp"

#include <cmath>

namespace {

const float EXP_VOLUME_DYNAMIC_RANGE = 40.f;
const float EXP_VOLUME_POWER = pow(10.f, EXP_VOLUME_DYNAMIC_RANGE / 20.f);
const float EXP_VOLUME_A = 1.f / EXP_VOLUME_POWER;
const float EXP_VOLUME_B = log(EXP_VOLUME_POWER);

} // namespace

namespace Audio {

float CovertToExpVolume(float value) {
    if(value <= 0.001) {
        return 0.f;
    }
    return EXP_VOLUME_A * exp(value * EXP_VOLUME_B);
}

} // namespace Audio