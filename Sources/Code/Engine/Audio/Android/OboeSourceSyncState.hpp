#ifndef __OBOE_SOURCE_STATE_HPP__
#define __OBOE_SOURCE_STATE_HPP__

#include <atomic>

struct OboeSourceState {
    float gain;
    bool isLooped;
    bool isPaused;
    bool isStopped;

    OboeSourceState() :
        gain(1.f),
        isLooped(false),
        isPaused(false),
        isStopped(false) {}
};

class OboeSourceSyncState {
public:

    OboeSourceSyncState();
    ~OboeSourceSyncState();

    void requestGain(float newGain);
    void requestLooping(bool flag);
    void requestStop(bool flag);
    void requestPaused(bool flag);
    void requestStopped(bool flag);

    void syncRead();

    bool isEndConfirmed() const;
    void confirmStart();
    void confirmEnd();

    const OboeSourceState& getReadState() const;

private:

    OboeSourceSyncState(const OboeSourceSyncState&) = delete;
    OboeSourceSyncState& operator=(const OboeSourceSyncState&) = delete;

    void waitEndOfLastUpdate();

private:

    OboeSourceState readState;
    OboeSourceState writeState;
    std::atomic<bool> isUpdated;
    std::atomic<bool> isEnded;
};

#endif /* __OBOE_SOURCE_STATE_HPP__ */