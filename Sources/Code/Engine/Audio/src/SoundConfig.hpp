#ifndef __MIX_CONFIG_HPP__
#define __MIX_CONFIG_HPP__

struct MixConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    MixConfig();

public:

    int outSampleRate;
    int maxSources;
    int channels;
    int buffersCount;
    int samplesPerBuffer;
    int samplesPerBurst;
};

struct SoundConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    MixConfig mixConfig;
    std::string soundEventTable;
};

bool CheckAndPrintMixConfig(const MixConfig& mixConfig);

#endif /* __MIX_CONFIG_HPP__ */