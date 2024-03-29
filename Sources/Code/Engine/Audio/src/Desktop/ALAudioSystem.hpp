#ifndef __AL_AUDIO_SYSTEM_HPP__
#define __AL_AUDIO_SYSTEM_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETTasks.hpp"
#include "MixGraph/MixGraph.hpp"
#include "Audio/ETSoundManagers.hpp"
#include "Audio/ETAudioSystem.hpp"

struct ALCdevice;
struct ALCcontext;
class ALSoundSource;

class ALAudioSystem : public SystemLogic,
    public ETNode<ETSoundUpdateTask>,
    public ETNode<ETSoundPlayManager>,
    public ETNode<ETAudioSystem> {
public:

    ALAudioSystem();
    virtual ~ALAudioSystem();

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

private:

    void updateMixGraph();
    bool initSoundContext();
    bool initAlSource();

private:

    ALCdevice* alcDevice;
    ALCcontext* alcContext;
    MixGraph mixGraph;
    std::unique_ptr<float[]> mixBuffer;
    std::unique_ptr<unsigned int[]> queueBufferIds;
    std::unique_ptr<unsigned int[]> alBufferIds;
    unsigned int alSourceId;
    bool alSourcePlaying;
};

#endif /* __AL_AUDIO_SYSTEM_HPP__ */