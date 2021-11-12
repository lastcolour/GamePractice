#ifndef __SOUND_DATA_HPP__
#define __SOUND_DATA_HPP__

#include <atomic>

class SoundData {
public:

    SoundData();
    ~SoundData();

    bool isLoaded() const;
    bool isLoading() const;
    void setLoading();
    void setLoaded(Memory::Buffer& data);
    bool tryFree();
    void addUseRef();
    void removeUseRef();
    bool hasUseRef() const;

public:

    Memory::Buffer data;
    const char* fileName;

private:

    SoundData(const SoundData&) = delete;
    SoundData& operator=(const SoundData&) = delete;

private:

    enum LoadState {
        Loading,
        Loaded,
        Released
    };

private:

    std::atomic<LoadState> loadState;
    std::atomic<int> useRefCount;
};

#endif /* __SOUND_DATA_HPP__ */