#ifndef __SOUND_STREAM_HPP__
#define __SOUND_STREAM_HPP__

#include "Audio/ETSound.hpp"

#include <memory>

class OggDataStream;
class SourceNode;

enum SoundStreamState {
    Stop = 0,
    Play,
    Pause,
    MixLast
};

class SoundStream : public Sound {
public:

    SoundStream(std::unique_ptr<OggDataStream>&& oggDataStream);
    virtual ~SoundStream();

    void exclusiveMixTo(void* out, int channels, int samples);
    int getSampleRate() const;
    int getChannels() const;
    void setMixNode(SourceNode* node);

    // Sound
    void play() override;
    void stop() override;
    void pause() override;
    void resume() override;
    void setLooped(bool flag) override;
    void setVolume(float newVolume) override;
    float getVolume() const override;
    bool isPlaying() const override;
    bool isPaused() const override;
    bool isLooped() const override;

private:

    void freeMixNode();

private:

    std::unique_ptr<OggDataStream> oggData;
    SourceNode* mixNode;
    float volume;
    SoundStreamState state;
    bool looped;
};

#endif /* __SOUND_STREAM_HPP__ */