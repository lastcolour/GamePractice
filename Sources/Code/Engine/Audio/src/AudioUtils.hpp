#ifndef __AUDIO_UTILS_HPP__
#define __AUDIO_UTILS_HPP__

#include <cstdint>

namespace Audio {

float CovertToExpVolume(float value);

void ConverFloatToInt16(float* from, int16_t* to, int samples);

} // namespace Audio

#endif /* __AUDIO_UTILS_HPP__ */