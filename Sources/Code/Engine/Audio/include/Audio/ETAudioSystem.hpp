#ifndef __ET_AUDIO_SYSTEM_HPP__
#define __ET_AUDIO_SYSTEM_HPP__

struct ETAudioSystem {
    virtual ~ETAudioSystem() = default;
    virtual void ET_setMasterVolume(float newVolume) = 0;
    virtual float ET_getMasterVolume() const = 0;
};

#endif /* __ET_AUDIO_SYSTEM_HPP__ */