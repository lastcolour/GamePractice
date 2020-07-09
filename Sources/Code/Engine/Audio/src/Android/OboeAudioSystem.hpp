#ifndef __OBOE_AUDIO_SYSTEM_HPP__
#define __OBOE_AUDIO_SYSTEM_HPP__

#include "Core/SystemLogic.hpp"

#include <oboe/Oboe.h>

class OboeAudioSystem : public SystemLogic,
    public oboe::AudioStreamCallback,
    public ETNode<ETSoundPlayManager> {
public:

    OboeAudioSystem();
    virtual ~OboeAudioSystem();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // oboe::AudioStreamCallback
    oboe::DataCallbackResult onAudioReady(oboe::AudioStream* outStream, void* audioData, int32_t numFrames) override;
    void onErrorAfterClose(oboe::AudioStream* stream, oboe::Result res) override;

private:

    bool initOboeMixer();
    bool initOboeStream();
    bool initOboeSources();

    void stopOboeStream();

private:

    oboe::AudioStream* oboeStream;
};

#endif /* __OBOE_AUDIO_SYSTEM_HPP__ */