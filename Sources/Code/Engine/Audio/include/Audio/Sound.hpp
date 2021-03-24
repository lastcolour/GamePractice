#ifndef __SOUND_HPP__
#define __SOUND_HPP__

class SoundData;
class SoundProxy;

enum class ESoundGroup {
    Music = 0,
    Game,
    UI
};

class Sound {
public:

    static void Reflect(ReflectContext& ctx);

public:

    Sound();
    Sound(Sound&& other);
    Sound& operator=(Sound&& other);
    ~Sound();

    void fadeInPlay(float duration);
    void fadeOutStop(float duration);
    void play();
    void stop();
    void pause();
    void resume();
    void setLooped(bool flag);
    bool getLooped() const;
    void setVolume(float newVolume);
    float getVolume() const;
    bool isPlaying() const;
    bool isLooped() const;
    bool isValid() const;
    ESoundGroup getGroup() const;
    void setGroup(ESoundGroup newGroup);
    void setFile(const char* fileName);
    const char* getFile() const;

private:

    Sound(const Sound&) = delete;
    Sound& operator=(const Sound& other) = delete;

private:

    SoundProxy* proxy;
    float volume;
    ESoundGroup group;
    bool looped;
};

#endif /* __SOUND_HPP__ */