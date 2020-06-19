#ifndef __SOUND_HPP__
#define __SOUND_HPP__

#include "SoundSource.hpp"

#include <memory>

class Sound : public SoundSourceController {
public:

    Sound(std::unique_ptr<OggDataStream>&& stream);
    ~Sound();

    void play(bool looped);
    void pause();
    void resume();
    void stop();
    bool isPlaying() const;

    void setVolume(float volume);

    // SoundSourceController
    void detachFromSource() override;

public:

    std::unique_ptr<OggDataStream> dataStream;
    SoundSource* soundSource;
};

#endif /* __SOUND_HPP__ */