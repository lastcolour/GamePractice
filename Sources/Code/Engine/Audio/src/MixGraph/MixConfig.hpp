#ifndef __MIX_CONFIG_HPP__
#define __MIX_CONFIG_HPP__
struct MixConfig {
    int outSampleRate {44100};
    int maxSources {32};
    int channels {2};
    int buffersCount {4};
    int samplesPerBuffer {0};
    int samplesPerBurst {0};
    float buffersTime {0.1f};
};

bool CheckMixConfig(const MixConfig& config);

#endif /* __MIX_CONFIG_HPP__ */