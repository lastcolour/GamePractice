#ifndef __OBOE_AUDIO_SYSTEM_HPP__
#define __OBOE_AUDIO_SYSTEM_HPP__

#include "Core/ETPrimitives.hpp"
#include "Core/SystemLogic.hpp"
#include "Audio/ETAudioInterfaces.hpp"

class OboeAudioSystem : public SystemLogic,
     public ETNode<ETSoundSourceManager> {
public:

    OboeAudioSystem();
    virtual ~OboeAudioSystem();

    // SystemLogic
    bool init() override;
    void deinit() override;

    SoundSource* ET_getFreeSource() override;
    void ET_returnSoundSource(SoundSource* soundSoruce) override;

private:

};

#endif /* __OBOE_AUDIO_SYSTEM_HPP__ */