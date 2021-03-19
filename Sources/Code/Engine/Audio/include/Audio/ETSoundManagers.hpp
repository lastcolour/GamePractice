#ifndef __ET_SOUND_PLAY_MANAGER_HPP__
#define __ET_SOUND_PLAY_MANAGER_HPP__

#include "Audio/Sound.hpp"

struct ETSoundPlayManager {
    virtual ~ETSoundPlayManager() = default;
    virtual bool ET_attachToMixNode(SoundProxy& proxyNode) = 0;
};

struct ETSoundEventManager {
    virtual ~ETSoundEventManager() = default;
    virtual void ET_emitEvent(const char* eventName) = 0;
};

struct ETSoundDataManager {
    virtual ~ETSoundDataManager() = default;
    virtual SoundProxy* ET_createSoundProxy() = 0;
    virtual void ET_removeSoundProxy(SoundProxy* state) = 0;
    virtual std::shared_ptr<SoundData> ET_loadSoundData(const char* fileName) = 0;
};

#endif /* __ET_SOUND_PLAY_MANAGER_HPP__ */