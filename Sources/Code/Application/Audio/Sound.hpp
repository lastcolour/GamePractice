#ifndef __SOUND_HPP__
#define __SOUND_HPP__

#include "Audio/SoundSource.hpp"

#include <memory>

class Sound : public SoundSourceController {
public:

    Sound();
    ~Sound();

    void play(bool looped);
    void pause();
    void resume();
    void stop();
    bool isPlaying() const;

    void setVolume(float volume);

    // SoundSourceController
    void detachFromSource() override;
    OggDataStream* getDataStream() override;

public:

    std::unique_ptr<OggDataStream> dataStream;
    SoundSource* soundSource;
};

#endif /* __SOUND_HPP__ */