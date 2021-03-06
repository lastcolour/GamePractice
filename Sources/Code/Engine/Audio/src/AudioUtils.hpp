#ifndef __AUDIO_UTILS_HPP__
#define __AUDIO_UTILS_HPP__

#include <cstdint>

namespace Audio {

float ConvertToExpVolume(float value);

void ConverFloatsToInt16(float* from, int16_t* to, int samples);

} // namespace Audio

#endif /* __AUDIO_UTILS_HPP__ */