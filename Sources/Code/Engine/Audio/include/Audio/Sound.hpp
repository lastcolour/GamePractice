#ifndef __SOUND_HPP__
#define __SOUND_HPP__

class Sound {
public:

    Sound();
    Sound(EntityId soundNodeId);
    Sound(Sound&& other);
    Sound& operator=(Sound&& other);
    ~Sound();

    // Sound
    void play();
    void stop();
    void pause();
    void resume();
    void setLooped(bool flag);
    void setVolume(float newVolume);
    float getVolume() const;
    bool isPlaying() const;
    bool isPaused() const;
    bool isLooped() const;
    bool isValid() const;

private:

    Sound(const Sound&) = delete;
    Sound& operator=(const Sound& other) = delete;

private:

    EntityId nodeId;
    float volume;
    bool looped;
};

#endif /* __SOUND_HPP__ */