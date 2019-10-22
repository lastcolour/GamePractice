#ifndef __AL_AUDIO_SYSTEM_HPP__
#define __AL_AUDIO_SYSTEM_HPP__

#include "Core/SystemLogic.hpp"
#include "Audio/ETAudioInterfaces.hpp"

struct ALCdevice;
struct ALCcontext;
class ALSoundSource;

class ALAudioSystem : public SystemLogic,
    public ETNode<ETAudioSystem> {
public:

    ALAudioSystem();
    virtual ~ALAudioSystem();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETAudioSystem
    std::vector<SoundSource*> ET_getSourcesToManage() override;

private:

    bool initSoundContext();
    bool initSoundSources();

private:

    ALCdevice* alcDevice;
    ALCcontext* alcContext;
    std::vector<std::unique_ptr<ALSoundSource>> alSources;
};

#endif /* __AL_AUDIO_SYSTEM_HPP__ */