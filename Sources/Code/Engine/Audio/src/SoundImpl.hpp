#ifndef __SOUND_IMPL_HPP__
#define __SOUND_IMPL_HPP__

#include "Audio/ETSound.hpp"
#include "SoundStream.hpp"
#include "Core/Buffer.hpp"

#include <memory>

class SourceNode;

class SoundImpl : public Sound,
    public SoundStream {
public:

    SoundImpl(Buffer& data);
    virtual ~SoundImpl();

    // SoundStream
    unsigned int getSamplesOffset() const override;
    void setMixNode(SourceNode* node) override;
    bool isMixLooped() const override;
    float getMixVolume() const override;
    bool isEvent() const override;
    Buffer& getData() override;

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

    Buffer soundData;
    SourceNode* mixNode;
    float volume;
    unsigned int samplesOffset;
    bool looped;
};

#endif /* __SOUND_IMPL_HPP__ */