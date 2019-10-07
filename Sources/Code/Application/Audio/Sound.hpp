#ifndef __SOUND_HPP__
#define __SOUND_HPP__

#include <memory>

class SoundSource;
class OggDataStream;

class Sound {
public:

    Sound();
    ~Sound();

    void play();
    void pause();
    void resume();
    void stop();
    bool isPlaying() const;

public:

    std::shared_ptr<OggDataStream> dataStream;
    SoundSource* soundSource;
};

#endif /* __SOUND_HPP__ */