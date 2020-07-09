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

private:

    Buffer loadSoundBuffer(const char* soundName);

private:

    std::unordered_map<std::string, Buffer> buffers;
};

#endif /* __SOUND_MANAGER_HPP__ */