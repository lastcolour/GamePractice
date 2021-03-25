#ifndef __SOUND_DATA_HPP__
#define __SOUND_DATA_HPP__

class SoundData {
public:

    SoundData();
    ~SoundData();

    void requestLoad();
    bool isLoaded() const;
    bool isLoading() const;
    bool isLoadRequired();
    void setLoading();
    void setLoaded(Buffer& data);
    bool tryFree();
    void setKeepInMemory(bool flag);
    void addMixNodeRef();
    void removeMixNodeRef();

public:

    Buffer data;
    const char* fileName;

private:

    SoundData(const SoundData&) = delete;
    SoundData& operator=(const SoundData&) = delete;

private:

    enum LoadState {
        LoadRequested = 0,
        Loading,
        Loaded,
        Released
    };

private:

    LoadState loadState;
    int mixNodesRefCount;
    bool keepInMemory;
};

#endif /* __SOUND_DATA_HPP__ */