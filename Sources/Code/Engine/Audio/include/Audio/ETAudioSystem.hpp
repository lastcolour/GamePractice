#ifndef __ET_AUDIO_SYSTEM_HPP__
#define __ET_AUDIO_SYSTEM_HPP__

class EqualizerSetup;

enum class ESoundGroup {
    Music = 0,
    Game,
    UI
};

struct ETAudioSystem {
    virtual ~ETAudioSystem() = default;
    virtual void ET_setEqualizer(ESoundGroup soundGroup, const EqualizerSetup& eqSetup) = 0;
    virtual void ET_setMasterVolume(float newVolume) = 0;
    virtual float ET_getMasterVolume() const = 0;
};

#endif /* __ET_AUDIO_SYSTEM_HPP__ */