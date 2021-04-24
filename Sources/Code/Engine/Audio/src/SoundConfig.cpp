#include "SoundConfig.hpp"

void MixConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<MixConfig>("MixConfig")) {
        classInfo->addField("outSampleRate", &MixConfig::outSampleRate);
        classInfo->addField("maxSources", &MixConfig::maxSources);
        classInfo->addField("channels", &MixConfig::channels);
        classInfo->addField("buffersCount", &MixConfig::buffersCount);
        classInfo->addField("samplesPerBuffer", &MixConfig::samplesPerBuffer);
    }
}

MixConfig::MixConfig() :
    outSampleRate(44100),
    maxSources(32),
    channels(2),
    buffersCount(4),
    samplesPerBuffer(800),
    samplesPerBurst(0) {
}

void SoundConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<SoundConfig>("SoundConfig")) {
        classInfo->addField("mix_config", &SoundConfig::mixConfig);
        classInfo->addField("sound_event_table", &SoundConfig::soundEventTable);
    }
}

bool CheckAndPrintMixConfig(const MixConfig& mixConfig) {
    bool res = true;
    if(mixConfig.outSampleRate <= 0) {
        LogError("[CheckMixConfig] Invalid sample rate: %d", mixConfig.outSampleRate);
        res = false;
    }
    if(mixConfig.maxSources <= 0) {
        LogError("[CheckMixConfig] Invalid max sources: %d", mixConfig.maxSources);
        res = false;
    }
    if(mixConfig.channels != 1 && mixConfig.channels != 2) {
        LogError("[CheckMixConfig] Invalid channels: %d", mixConfig.channels);
        res = false;
    }
    if(mixConfig.buffersCount < 0) {
        LogError("[CheckMixConfig] Invalid buffers count: %d", mixConfig.buffersCount);
        res = false;
    }
    if(mixConfig.samplesPerBuffer <= 0) {
        LogError("[CheckMixConfig] Invalid samples per buffer: %d", mixConfig.samplesPerBuffer);
        res = false;
    }
    if(mixConfig.samplesPerBurst < 0) {
        LogError("[CheckMixConfig] Invalid sample per burst: %d", mixConfig.samplesPerBurst);
        res = false;
    }

    LogInfo("[CheckAndPrintMixConfig] MixConfig:");
    LogInfo("[CheckAndPrintMixConfig] - outSampleRate: %d", mixConfig.outSampleRate);
    LogInfo("[CheckAndPrintMixConfig] - maxSources: %d", mixConfig.maxSources);
    LogInfo("[CheckAndPrintMixConfig] - channels: %d", mixConfig.channels);
    LogInfo("[CheckAndPrintMixConfig] - buffersCount: %d", mixConfig.buffersCount);
    LogInfo("[CheckAndPrintMixConfig] - samplesPerBuffer: %d", mixConfig.samplesPerBuffer);
    LogInfo("[CheckAndPrintMixConfig] - samplesPerBurst: %d", mixConfig.samplesPerBurst);

    return res;
}