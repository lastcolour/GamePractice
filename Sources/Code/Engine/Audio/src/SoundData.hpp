#ifndef __SOUND_DATA_HPP__
#define __SOUND_DATA_HPP__

class SoundData {
public:

    const char* fileName;
    Buffer data;
    std::atomic<bool> isLoaded;

public:

    SoundData();
};

#endif /* __SOUND_DATA_HPP__ */