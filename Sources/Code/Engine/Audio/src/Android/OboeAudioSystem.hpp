#ifndef __OBOE_AUDIO_SYSTEM_HPP__
#define __OBOE_AUDIO_SYSTEM_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETTasks.hpp"
#include "Audio/ETSoundManagers.hpp"
#include "Audio/ETAudioSystem.hpp"
#include "MixGraph/MixGraph.hpp"
#include "AudioBufferQueue.hpp"
#include "Platform/ETSurface.hpp"
#include "Platform/Android/ETAndroidInterfaces.hpp"
#include "Fader.hpp"

#include <atomic>

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
    bool ET_startSound(SoundProxy& proxyNode, float duration, bool isEvent) override;
    void ET_stopSound(SoundProxy& proxyNode, float duration, bool resetOffset) override;

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

    void updateMixGraph();
    bool initMixer();
    bool initOboeStream();
    void stopOboeStream();

private:

    MixGraph mixGraph;
    std::unique_ptr<float[]> mixBuffer;
    AudioBufferQueue bufferQueue;
    Fader fader;
    oboe::AudioStream* oboeStream;
    std::atomic<bool> canMix;
};

#endif /* __OBOE_AUDIO_SYSTEM_HPP__ */