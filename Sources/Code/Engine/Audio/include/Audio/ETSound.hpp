#ifndef __ET_SOUND_HPP__
#define __ET_SOUND_HPP__

#include "Audio/Sound.hpp"
#include "Audio/SoundEvent.hpp"

struct ETSoundPlayer {
    virtual ~ETSoundPlayer() = default;
    virtual void ET_play() = 0;
    virtual void ET_stop() = 0;
};

struct ETSoundManager {
    virtual ~ETSoundManager() = default;
    virtual Sound ET_createSound(const char* soundName) = 0;
    virtual SoundEvent ET_createEvent(const char* soundName) = 0;
};

#endif /* __ET_SOUND_HPP__ */ 