#include "MixGraph/MixConfig.hpp"
#include "Core/ETLogger.hpp"

bool CheckAndPrintMixConfig(const MixConfig& config) {
    bool res = true;
    if(config.outSampleRate <= 0) {
        LogError("[CheckMixConfig] Invalid sample rate: %d", config.outSampleRate);
        res = false;
    }
    if(config.maxSources <= 0) {
        LogError("[CheckMixConfig] Invalid max sources: %d", config.maxSources);
        res = false;
    }
    if(config.channels != 1 && config.channels != 2) {
        LogError("[CheckMixConfig] Invalid channels: %d", config.channels);
        res = false;
    }
    if(config.buffersCount < 0) {
        LogError("[CheckMixConfig] Invalid buffers count: %d", config.buffersCount);
        res = false;
    }
    if(config.samplesPerBuffer <= 0) {
        LogError("[CheckMixConfig] Invalid samples per buffer: %d", config.samplesPerBuffer);
        res = false;
    }
    if(config.samplesPerBurst < 0) {
        LogError("[CheckMixConfig] Invalid sample per burst: %d", config.samplesPerBurst);
        res = false;
    }

    LogInfo("[CheckAndPrintMixConfig] MixConfig:");
    LogInfo("[CheckAndPrintMixConfig] - outSampleRate: %d", config.outSampleRate);
    LogInfo("[CheckAndPrintMixConfig] - maxSources: %d", config.maxSources);
    LogInfo("[CheckAndPrintMixConfig] - channels: %d", config.channels);
    LogInfo("[CheckAndPrintMixConfig] - buffersCount: %d", config.buffersCount);
    LogInfo("[CheckAndPrintMixConfig] - samplesPerBuffer: %d", config.samplesPerBuffer);
    LogInfo("[CheckAndPrintMixConfig] - samplesPerBurst: %d", config.samplesPerBurst);

    return res;
}