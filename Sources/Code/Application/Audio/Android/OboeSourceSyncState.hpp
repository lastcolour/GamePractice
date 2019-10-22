#ifndef __OBOE_SOURCE_STATE_HPP__
#define __OBOE_SOURCE_STATE_HPP__

#include <atomic>

struct OboeSourceState {
    float gain;
    bool isLooped;
    bool isPaused;
    bool isStopped;
};

class OboeSourceSyncState {
public:

    OboeSourceSyncState();
    ~OboeSourceSyncState();

    void requestLooping(bool flag);
    void requestStop(bool flag);
    void requestGain(float newGain);
    void requestPaused(bool flag);
    void requestStopped(bool flag);

    void sync();

    const OboeSourceState* getReadState() const;
    const OboeSourceState* getWriteState() const;

private:

    OboeSourceState* getReadData();
    OboeSourceState* getWriteData();

private:

    OboeSourceState states[2];
    std::atomic<bool> isNeedUpdate;
    std::atomic<bool> isUpdated;
    std::atomic<bool> isReadFirst;

private:

    OboeSourceSyncState(const OboeSourceSyncState&) = delete;
    OboeSourceSyncState& operator=(const OboeSourceSyncState&) = delete;
};

#endif /* __OBOE_SOURCE_STATE_HPP__ */