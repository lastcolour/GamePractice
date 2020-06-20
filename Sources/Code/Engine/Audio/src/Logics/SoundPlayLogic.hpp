#ifndef __SOUND_PLAY_LOGIC_HPP__
#define __SOUND_PLAY_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Audio/ETAudioInterfaces.hpp"
#include "Sound.hpp"

class ReflectContext;

class SoundPlayLogic : public EntityLogic,
    public ETNode<ETSoundPlayer> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    SoundPlayLogic();
    virtual ~SoundPlayLogic();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETSoundPlayer
    void ET_setSound(const char* newSoundName) override;
    void ET_play() override;
    void ET_pause() override;
    void ET_resume() override;
    void ET_stop() override;

private:

    std::unique_ptr<Sound> sound;
    float volume;
    bool looped;
    bool autoStart;
};

#endif /* __SOUND_PLAY_LOGIC_HPP__ */