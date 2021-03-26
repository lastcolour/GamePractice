#ifndef __SOUND_DATA_HPP__
#define __SOUND_DATA_HPP__

class SoundData {
public:

    SoundData();
    ~SoundData();

    void requestLoad();
    bool isLoaded() const;
    bool canStartLoading();
    void setLoaded(Buffer& data);
    bool tryFree();
    void addUseRef();
    void removeUseRef();

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

    std::atomic<LoadState> loadState;
    std::atomic<int> useRefCount;
};

#endif /* __SOUND_DATA_HPP__ */