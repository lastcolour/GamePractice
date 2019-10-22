#include "Audio/Android/OboeSourceSyncState.hpp"

OboeSourceSyncState::OboeSourceSyncState() {
}

OboeSourceSyncState::~OboeSourceSyncState() {
}

void OboeSourceSyncState::requestLooping(bool flag) {
    getWriteData()->isLooped = flag;
    isUpdated.store(true);
}

void OboeSourceSyncState::requestGain(float newGain) {
    getWriteData()->gain = newGain;
    isUpdated.store(true);
}

void OboeSourceSyncState::requestPaused(bool flag) {
    getWriteData()->isPaused = flag;
    isUpdated.store(true);
}

void OboeSourceSyncState::requestStopped(bool flag) {
    getWriteData()->isStopped = flag;
    isUpdated.store(true);
}

void OboeSourceSyncState::sync() {
    if(isReadFirst.load()) {
    }
}

OboeSourceState* OboeSourceSyncState::getWriteData() {
    if(isReadFirst.load()) {
        return &states[1];
    }
    return &states[0];
}

OboeSourceState* OboeSourceSyncState::getReadData() {
    if(isReadFirst.load()) {
        return &states[1];
    }
    return &states[0];
}

const OboeSourceState* OboeSourceSyncState::getWriteState() const {
    if(isReadFirst.load()) {
        return &states[1];
    }
    return &states[0];
}

const OboeSourceState* OboeSourceSyncState::getReadState() const {
    if(isReadFirst.load()) {
        return &states[0];
    }
    return &states[1];
}