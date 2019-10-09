#ifndef __AUDIO_SYSTEM_HPP__
#define __AUDIO_SYSTEM_HPP__

#include "Core/SystemLogic.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Audio/SoundSource.hpp"
#include "Audio/ETAudioInterfaces.hpp"

struct ALCdevice;
struct ALCcontext;

class AudioSystem : public SystemLogic,
    public ETNode<ETTimerEvents>,
    public ETNode<ETSoundSourceManager> {
public:

    AudioSystem();
    virtual ~AudioSystem();

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

    ALCdevice* alcDevice;
    ALCcontext* alcContext;

    std::vector<SoundSource> sources;
    std::vector<ESourceState> sourceStateMap;
};

#endif /* __AUDIO_SYSTEM_HPP__ */