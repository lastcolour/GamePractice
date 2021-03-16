#ifndef __SOUND_EQUALIZER_LOGIC_HPP__
#define __SOUND_EQUALIZER_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Audio/ETEqualizer.hpp"
#include "Audio/ETAudioSystem.hpp"
#include "Filters/EqualizerSetup.hpp"

class SoundEqualizerLogic : public EntityLogic,
    public ETNode<ETEqualizer> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    SoundEqualizerLogic();
    virtual ~SoundEqualizerLogic();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETEqualizer
    void ET_enable() override;
    void ET_disable() override;

private:

    ESoundGroup soundGroup;
    EqualizerSetup eqSetup;
    bool autoEnable;
    bool isEnabled;
};

#endif /* __SOUND_EQUALIZER_LOGIC_HPP__ */