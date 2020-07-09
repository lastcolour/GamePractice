#ifndef __ET_SOUND_HPP__
#define __ET_SOUND_HPP__

#include <memory>

struct Sound {
    virtual ~Sound() = default;
    virtual void play() = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual void setLooped(bool flag) = 0;
    virtual bool isLooped() const = 0;
    virtual void setVolume(float newVolume) = 0;
    virtual float getVolume() const = 0;
    virtual bool isPlaying() const = 0;
    virtual bool isPaused() const = 0;
};

struct ETSoundPlayer {
    virtual ~ETSoundPlayer() = default;
    virtual void ET_play() = 0;
    virtual void ET_stop() = 0;
};

struct ETSoundManager {
    virtual ~ETSoundManager() = default;
    virtual std::unique_ptr<Sound> ET_createSound(const char* soundName) = 0;
};

#endif /* __ET_SOUND_HPP__ */