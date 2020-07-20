#ifndef __SOUND_MANAGER_HPP__
#define __SOUND_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Audio/ETSound.hpp"
#include "Core/ETPrimitives.hpp"
#include "Core/Buffer.hpp"

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
    std::shared_ptr<SoundEvent> ET_createEvent(const char* soundName) override;

private:

    bool loadEvents();
    Buffer loadSoundBuffer(const char* soundName);

private:

    std::unordered_map<std::string, Buffer> buffers;
    std::unordered_map<std::string, std::shared_ptr<SoundEvent>> eventMap;
};

#endif /* __SOUND_MANAGER_HPP__ */