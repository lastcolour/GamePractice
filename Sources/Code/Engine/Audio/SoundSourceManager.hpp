#ifndef __SOUND_SOURCE_MANAGER_HPP__
#define __SOUND_SOURCE_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Audio/ETAudioInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"

class SoundSourceManager : public SystemLogic,
    public ETNode<ETSoundSourceManager>,
    public ETNode<ETTimerEvents> {
public:

    SoundSourceManager();
    virtual ~SoundSourceManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETTimerEvents
    void ET_onTick(float dt) override;

    // ETSoundSourceManager
    SoundSource* ET_getFreeSource() override;
    void ET_returnSoundSource(SoundSource* retSoundSource) override;

private:

    enum class ESourceState {
        Free = 0,
        Busy
    };

private:

    std::vector<SoundSource*> sources;
    std::vector<ESourceState> sourceStateMap;
};

#endif /* __SOUND_SOURCE_MANAGER_HPP__ */