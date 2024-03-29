#ifndef __ET_SOUND_PLAY_MANAGER_HPP__
#define __ET_SOUND_PLAY_MANAGER_HPP__

#include "Audio/Sound.hpp"

enum class ESoundCommand {
    Start = 0,
    Pause,
    Resume,
    Stop,
    Emit
};

struct ETSoundPlayManager {
    virtual ~ETSoundPlayManager() = default;
    virtual bool ET_startSound(SoundProxy& proxyNode, float duration, bool isEvent) = 0;
    virtual void ET_stopSound(SoundProxy& proxyNode, float duration, bool resetOffset) = 0;
};

struct ETSoundEventManager {
    virtual ~ETSoundEventManager() = default;
    virtual void ET_emitEvent(const char* eventName) = 0;
};

struct ETSoundDataManager {
    virtual ~ETSoundDataManager() = default;
    virtual SoundProxy* ET_createSoundProxy() = 0;
    virtual void ET_removeSoundProxy(SoundProxy* proxy) = 0;
    virtual std::shared_ptr<SoundData> ET_createSoundData(const std::string& fileName) = 0;
    virtual void ET_addSoundUpdate(std::function<void(void)> func) = 0;
    virtual void ET_addSoundCmd(SoundProxy* proxyNode, ESoundCommand cmd, float duration) = 0;
    virtual void ET_updatePendingStarts() = 0;
    virtual void ET_updateSoundData() = 0;
};

#endif /* __ET_SOUND_PLAY_MANAGER_HPP__ */