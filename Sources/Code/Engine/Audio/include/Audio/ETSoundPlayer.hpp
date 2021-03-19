#ifndef __ET_SOUND_HPP__
#define __ET_SOUND_HPP__

struct ETSoundPlayer {
    virtual ~ETSoundPlayer() = default;
    virtual void ET_play() = 0;
    virtual void ET_stop() = 0;
};

#endif /* __ET_SOUND_HPP__ */ 