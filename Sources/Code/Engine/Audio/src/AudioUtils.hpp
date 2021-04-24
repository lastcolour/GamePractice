#ifndef __AUDIO_UTILS_HPP__
#define __AUDIO_UTILS_HPP__

#include <cstdint>

#include "SoundConfig.hpp"

namespace Audio {

void ConverFloatsToInt16(float* from, int16_t* to, int samples);

MixConfig& GetMixConfig();

} // namespace Audio

#endif /* __AUDIO_UTILS_HPP__ */