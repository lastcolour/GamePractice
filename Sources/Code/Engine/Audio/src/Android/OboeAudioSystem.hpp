#ifndef __OBOE_AUDIO_SYSTEM_HPP__
#define __OBOE_AUDIO_SYSTEM_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Core/ETTasks.hpp"
#include "Audio/ETSoundPlayManager.hpp"
#include "Audio/ETAudioSystem.hpp"
#include "MixGraph/MixGraph.hpp"
#include "AudioBufferQueue.hpp"
#include "Platform/ETSurface.hpp"
#include "Platform/Android/ETAndroidInterfaces.hpp"

#include <oboe/Oboe.h>

class OboeAudioSystem : public SystemLogic,
    public ETNode<ETSoundPlayManager>,
    public ETNode<ETSoundUpdateTask>,
    public ETNode<ETAudioSystem>,
    public ETNode<ETAndroidActivityEvents>,
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

    // ETAndroidActivityEvents
    void ET_onActivityEvent(ActivityEventType eventType) override;

    // oboe::AudioStreamCallback
    oboe::DataCallbackResult onAudioReady(oboe::AudioStream* outStream, void* audioData, int32_t numFrames) override;
    void onErrorAfterClose(oboe::AudioStream* stream, oboe::Result res) override;

private:

    bool initMixer();
    bool initOboeStream();
    void stopOboeStream();

private:

    MixGraph mixGrap;
    std::unique_ptr<float[]> mixBuffer;
    AudioBufferQueue bufferQueue;
    oboe::AudioStream* oboeStream;
};

#endif /* __OBOE_AUDIO_SYSTEM_HPP__ */