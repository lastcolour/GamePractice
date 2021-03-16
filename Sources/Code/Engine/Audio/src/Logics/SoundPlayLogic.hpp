#ifndef __SOUND_PLAY_LOGIC_HPP__
#define __SOUND_PLAY_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Audio/ETSound.hpp"

class SoundPlayLogic : public EntityLogic,
    public ETNode<ETSoundPlayer>,
    public ETNode<ETEntityEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    SoundPlayLogic();
    virtual ~SoundPlayLogic();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETSoundPlayer
    void ET_play() override;
    void ET_stop() override;

    // ETEntityEvents
    void ET_onTransformChanged(const Transform& newTm) override {}
    void ET_onLoaded() override;

private:

    void setSound(const char* soundName);

private:

    Sound sound;
    float volume;
    bool looped;
    bool autoStart;
};

#endif /* __SOUND_PLAY_LOGIC_HPP__ */