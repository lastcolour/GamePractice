#ifndef __ET_SOUND_NODE_HPP__
#define __ET_SOUND_NODE_HPP__

struct ETSoundNode {
    virtual ~ETSoundNode() = default;
    virtual void ET_play() = 0;
    virtual void ET_stop() = 0;
    virtual void ET_pause() = 0;
    virtual void ET_resume() = 0;
    virtual void ET_setLooped(bool flag) = 0;
    virtual void ET_setVolume(float newVolume) = 0;
    virtual bool ET_isPlaying() const = 0;
    virtual bool ET_isPaused() const = 0;
};

struct ETSoundEventNode {
    virtual ~ETSoundEventNode() = default;
    virtual void ET_emit() = 0;
};

#endif /* __ET_SOUND_NODE_HPP__ */