#ifndef __OBOE_SOUND_SOURCE_HPP__
#define __OBOE_SOUND_SOURCE_HPP__

#include "Audio/SoundSource.hpp"

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

};

#endif /* __OBOE_SOUND_SOURCE_HPP__ */