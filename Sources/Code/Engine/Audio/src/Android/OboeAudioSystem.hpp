#ifndef __OBOE_AUDIO_SYSTEM_HPP__
#define __OBOE_AUDIO_SYSTEM_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Core/ETTasks.hpp"
#include "Audio/ETSoundPlayManager.hpp"
#include "Audio/ETAudioSystem.hpp"

#include <oboe/Oboe.h>

class OboeAudioSystem : public SystemLogic,
    public ETNode<ETSoundPlayManager>,
    public ETNode<ETSoundUpdateTask>,
    public ETNode<ETAudioSystem>,
    public oboe::AudioStreamCallback {
public:

    OboeAudioSystem();
    virtual ~OboeAudioSystem();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETSoundUpdateTask
    void ET_updateSound() override;

    // ETSoundPlayManager
    bool ET_play(SoundStream* soundStream) override;

    // ETAudioSystem
    void ET_setEqualizer(ESoundGroup soundGroup, const EqualizerSetup& eqSetup) override;
    void ET_setMasterVolume(float newVolume) override;
    float ET_getMasterVolume() const override;

    // oboe::AudioStreamCallback
    oboe::DataCallbackResult onAudioReady(oboe::AudioStream* outStream, void* audioData, int32_t numFrames) override;
    void onErrorAfterClose(oboe::AudioStream* stream, oboe::Result res) override;

private:

    bool initOboeMixer();
    bool initOboeStream();
    void stopOboeStream();

private:

    oboe::AudioStream* oboeStream;
};

#endif /* __OBOE_AUDIO_SYSTEM_HPP__ */