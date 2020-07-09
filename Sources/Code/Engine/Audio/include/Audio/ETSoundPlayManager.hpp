#ifndef __ET_SOUND_PLAY_MANAGER_HPP__
#define __ET_SOUND_PLAY_MANAGER_HPP__

class SoundStream;

struct ETSoundPlayManager {
    virtual ~ETSoundPlayManager() = default;
    virtual bool ET_play(SoundStream* soundStream) = 0;
};

#endif /* __ET_SOUND_PLAY_MANAGER_HPP__ */