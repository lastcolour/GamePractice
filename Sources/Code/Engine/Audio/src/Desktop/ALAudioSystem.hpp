#ifndef __AL_AUDIO_SYSTEM_HPP__
#define __AL_AUDIO_SYSTEM_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETTasks.hpp"
#include "Core/ETPrimitives.hpp"
#include "MixGraph/MixGraph.hpp"
#include "Audio/ETSoundPlayManager.hpp"
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
    bool ET_play(SoundStream* soundStream) override;

    // ETAudioSystem
    void ET_setEqualizer(ESoundGroup soundGroup, const EqualizerSetup& eqSetup) override;
    void ET_setMasterVolume(float newVolume) override;
    float ET_getMasterVolume() const override;

private:

    bool initSoundContext();
    bool initAlSource();

private:

    ALCdevice* alcDevice;
    ALCcontext* alcContext;
    MixGraph mixGraph;
    std::unique_ptr<float[]> mixBuffer;
    std::unique_ptr<unsigned int[]> alBufferIds;
    unsigned int alSourceId;
    bool alSourcePlaying;
};

#endif /* __AL_AUDIO_SYSTEM_HPP__ */