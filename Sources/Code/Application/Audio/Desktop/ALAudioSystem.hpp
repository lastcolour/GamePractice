#ifndef __AL_AUDIO_SYSTEM_HPP__
#define __AL_AUDIO_SYSTEM_HPP__

#include "Core/SystemLogic.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Audio/Desktop/ALSoundSource.hpp"
#include "Audio/ETAudioInterfaces.hpp"

struct ALCdevice;
struct ALCcontext;

class ALAudioSystem : public SystemLogic,
    public ETNode<ETTimerEvents>,
    public ETNode<ETSoundSourceManager> {
public:

    ALAudioSystem();
    virtual ~ALAudioSystem();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETTimerEvents
    void ET_onTick(float dt) override;

    // ETSoundSourceManager
    SoundSource* ET_getFreeSource() override;
    void ET_returnSoundSource(SoundSource* soundSoruce) override;

private:

    bool initSoundContext();
    bool initSoundSources();

private:

    enum class ESourceState {
        Free = 0,
        Busy
    };

private:

    using ALSoundSourcePtrT = std::unique_ptr<ALSoundSource>;

private:

    ALCdevice* alcDevice;
    ALCcontext* alcContext;

    std::vector<ALSoundSourcePtrT> sources;
    std::vector<ESourceState> sourceStateMap;
};

#endif /* __AL_AUDIO_SYSTEM_HPP__ */