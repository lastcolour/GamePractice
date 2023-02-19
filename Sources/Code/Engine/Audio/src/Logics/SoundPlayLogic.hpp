#ifndef __SOUND_PLAY_LOGIC_HPP__
#define __SOUND_PLAY_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Audio/ETSoundPlayer.hpp"
#include "Audio/Sound.hpp"

class SoundPlayLogic : public EntityLogic,
    public ETNode<ETSoundPlayer> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    SoundPlayLogic();
    virtual ~SoundPlayLogic();

    // EntityLogic
    void init() override;
    void deinit() override;
    void onLoaded() override;

    // ETSoundPlayer
    void ET_play() override;
    void ET_stop() override;

private:

    Sound sound;
    bool autoStart;
};

#endif /* __SOUND_PLAY_LOGIC_HPP__ */