#ifndef __SOUND_PROXY_HPP__
#define __SOUND_PROXY_HPP__

#include "Audio/Sound.hpp"
#include "SoundData.hpp"

#include <atomic>

class SoundProxy {
public:

    SoundProxy();
    ~SoundProxy();

    SoundData* readData();
    ESoundGroup readGroup() const;
    float readVolume() const;
    bool readLooped() const;
    int readOffset() const;

    void writeOffset(int newOffset);

private:

    std::atomic<bool> looped;
    std::atomic<float> volume;
    std::atomic<int> offset;
};

#endif /* __SOUND_PROXY_HPP__ */