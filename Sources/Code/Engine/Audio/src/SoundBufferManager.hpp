#ifndef __SOUND_BUFFER_MANAGER_HPP__
#define __SOUND_BUFFER_MANAGER_HPP__

#include "Core/Buffer.hpp"

#include <unordered_map>
#include <string>

class SoundEventTable;

class SoundBufferManager {
public:

    SoundBufferManager();
    ~SoundBufferManager();

    void loadEvents(SoundEventTable& eventTable);
    Buffer loadSound(const std::string& fileName);

private:

    std::unordered_map<const char*, Buffer> buffers;
};

#endif /* __SOUND_BUFFER_MANAGER_HPP__ */