#ifndef __SOUND_PLAY_LOGIC_HPP__
#define __SOUND_PLAY_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Audio/ETAudioInterfaces.hpp"
#include "Audio/Sound.hpp"

class SoundPlayLogic : public EntityLogic,
    public ETNode<ETSoundPlayer> {
public:

    SoundPlayLogic();
    virtual ~SoundPlayLogic();

    // EntityLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETSoundPlayer
    void ET_setSound(const char* newSoundName) override;
    void ET_play() override;
    void ET_pause() override;
    void ET_resume() override;
    void ET_stop() override;

private:

    std::unique_ptr<Sound> sound;
    bool looped;
};

#endif /* __SOUND_PLAY_LOGIC_HPP__ */