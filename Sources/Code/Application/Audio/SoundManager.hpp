#ifndef __SOUND_MANAGER_HPP__
#define __SOUND_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Audio/ETAudioInterfaces.hpp"

#include <unordered_map>

class SoundManager : public SystemLogic,
    public ETNode<ETSoundManager> {
public:

    SoundManager();
    virtual ~SoundManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETSoundManager
    std::unique_ptr<Sound> ET_createSound(const char* soundName) override;

private:

    std::shared_ptr<Sound> loadSound(const char* soundName);

private:

    std::unordered_map<std::string, std::shared_ptr<Sound>> sounds;
};

#endif /* __SOUND_MANAGER_HPP__ */