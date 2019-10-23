#ifndef __OBOE_AUDIO_SYSTEM_HPP__
#define __OBOE_AUDIO_SYSTEM_HPP__

#include "Core/SystemLogic.hpp"
#include "Audio/ETAudioInterfaces.hpp"
#include "Audio/Android/OboeMixer.hpp"

#include <oboe/Oboe.h>

class OboeAudioSystem : public SystemLogic,
    public oboe::AudioStreamCallback,
    public ETNode<ETAudioSystem> {
public:

    OboeAudioSystem();
    virtual ~OboeAudioSystem();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // oboe::AudioStreamCallback
    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *outStream, void *audioData, int32_t numFrames) override;
    void onErrorAfterClose(oboe::AudioStream* stream, oboe::Result res) override;

    // ETAudioSystem
    std::vector<SoundSource*> ET_getSourcesToManage() override;

private:

    bool initOboeMixer();
    bool initOboeStream();
    bool initOboeSources();

private:

    OboeMixer mixer;
    oboe::AudioStream* oboeStream;
    std::vector<std::unique_ptr<OboeSoundSource>> oboeSources;
};

#endif /* __OBOE_AUDIO_SYSTEM_HPP__ */