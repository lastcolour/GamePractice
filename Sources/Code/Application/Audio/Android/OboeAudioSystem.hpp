#ifndef __OBOE_AUDIO_SYSTEM_HPP__
#define __OBOE_AUDIO_SYSTEM_HPP__

#include "Core/ETPrimitives.hpp"
#include "Core/SystemLogic.hpp"
#include "Audio/ETAudioInterfaces.hpp"
#include "Audio/Android/OboeMixer.hpp"
#include "Audio/Android/OboeSoundSource.hpp"

#include <oboe/Oboe.h>

class OboeAudioSystem : public SystemLogic,
     public ETNode<ETSoundSourceManager>,
     public oboe::AudioStreamCallback {
public:

    OboeAudioSystem();
    virtual ~OboeAudioSystem();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETSoundSourceManager
    SoundSource* ET_getFreeSource() override;
    void ET_returnSoundSource(SoundSource* retSoundSoruce) override;

    // oboe::AudioStreamCallback
    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *outStream, void *audioData, int32_t numFrames) override;

private:

    bool initOboeStream();
    bool initSoundSources();

private:

    enum class ESourceState {
        Free = 0,
        Busy
    };

private:

    OboeMixer mixer;
    std::vector<OboeSoundSource> sources;
    std::vector<ESourceState> sourceStateMap;
    oboe::AudioStream* oboeStream;
};

#endif /* __OBOE_AUDIO_SYSTEM_HPP__ */