#ifndef __ET_SOUND_PLAY_MANAGER_HPP__
#define __ET_SOUND_PLAY_MANAGER_HPP__

#include "Audio/Sound.hpp"

enum class ESoundCommand {
    Start = 0,
    Pause,
    Resume,
    Stop
};

struct ETSoundPlayManager {
    virtual ~ETSoundPlayManager() = default;
    virtual bool ET_addSoundCmd(SoundProxy* proxyNode, ESoundCommand cmd, float duration) = 0;
};

struct ETSoundEventManager {
    virtual ~ETSoundEventManager() = default;
    virtual void ET_emitEvent(const char* eventName) = 0;
};

struct ETSoundDataManager {
    virtual ~ETSoundDataManager() = default;
    virtual SoundProxy* ET_createSoundProxy() = 0;
    virtual void ET_cleanUpData() = 0;
    virtual void ET_setupSoundData(SoundProxy* soundProxy, const std::string& fileName) = 0;
};

#endif /* __ET_SOUND_PLAY_MANAGER_HPP__ */