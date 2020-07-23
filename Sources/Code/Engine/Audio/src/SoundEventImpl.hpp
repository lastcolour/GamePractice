#ifndef __SOUND_EVENT_IMPL_HPP__
#define __SOUND_EVENT_IMPL_HPP__

#include "SoundStream.hpp"
#include "Audio/ETSound.hpp"
#include "Core/Buffer.hpp"
#include "Core/TimePoint.hpp"

class SoundEventImpl : public SoundStream,
    public SoundEvent {
public:

    SoundEventImpl(float volumeVal, float nextDelayVal, Buffer& data);
    virtual ~SoundEventImpl();

    // SoundStream
    unsigned int getSamplesOffset() const override;
    void setMixNode(SourceNode* node) override;
    float getMixVolume() const override;
    bool isEvent() const override;
    bool isMixLooped() const override;
    Buffer& getData() override;

    // SoundEvent
    void emit() override;

private:

    Buffer soundData;
    float volume;
    float nextDelay;
    TimePoint lastPlayTime;
};

#endif /* __SOUND_EVENT_IMPL_HPP__ */