#include "AudioUtils.hpp"

namespace Audio {

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

MixConfig& GetMixConfig() {
    return Core::GetGlobal<SoundConfig>()->mixConfig;
}

} // namespace Audio