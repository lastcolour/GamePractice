#ifndef __SOUND_PLAY_LOGIC_HPP__
#define __SOUND_PLAY_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Audio/ETSound.hpp"
#include "Core/ETPrimitives.hpp"

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
    void ET_play() override;
    void ET_stop() override;

private:

    void setSound(const char* soundName);

private:

    std::unique_ptr<Sound> sound;
    float volume;
    bool looped;
    bool autoStart;
};

#endif /* __SOUND_PLAY_LOGIC_HPP__ */