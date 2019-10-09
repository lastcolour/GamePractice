#ifndef __SOUND_HPP__
#define __SOUND_HPP__

#include "Audio/SoundSourceController.hpp"

#include <memory>

class SoundSource;

class Sound : public SoundSourceController {
public:

    Sound();
    ~Sound();

    void play();
    void pause();
    void resume();
    void stop();
    bool isPlaying() const;

    // SoundSourceController
    void detachFromSource() override;
    OggDataStream* getDataStream() override;

public:

    std::unique_ptr<OggDataStream> dataStream;
    SoundSource* soundSource;
};

#endif /* __SOUND_HPP__ */