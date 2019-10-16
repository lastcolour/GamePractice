#ifndef __OBOE_SOUND_SOURCE_HPP__
#define __OBOE_SOUND_SOURCE_HPP__

#include "Audio/SoundSource.hpp"

#include <atomic>

#include <oboe/Oboe.h>

class OboeSoundSource : public SoundSource {
public:

    OboeSoundSource();
    virtual ~OboeSoundSource();

    // SoundSource
    void attachToController(SoundSourceController& newController) override;
    void stopStreaming() override;
    void pauseStreaming() override;
    void resumeStreaming() override;
    bool isStreaming() const override;
    void setGain(float newGain) override;
    void setLoop(bool loopFlag) override;
    bool isLooped() const  override;

    void fillBuffer(float* outBuffer, int numFrames, int channels);

private:

    enum class ESourceState {
        Normal = 0,
        WaitEnd,
        Ended
    };

private:

    SoundSourceController* controller;
    float gain;
    bool looped;
    ESourceState state;
    std::atomic<bool> isEnded;
};

#endif /* __OBOE_SOUND_SOURCE_HPP__ */