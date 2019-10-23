#include "Audio/Android/OboeSourceSyncState.hpp"

#include <cassert>
#include <thread>

OboeSourceSyncState::OboeSourceSyncState() :
    isUpdated(false),
    isEnded(true) {
}

OboeSourceSyncState::~OboeSourceSyncState() {
}

void OboeSourceSyncState::requestGain(float newGain) {
    waitEndOfLastUpdate();
    writeState.gain = newGain;
    isUpdated.store(true);
}

void OboeSourceSyncState::requestLooping(bool flag) {
    waitEndOfLastUpdate();
    writeState.isLooped = flag;
    isUpdated.store(true);
}

void OboeSourceSyncState::requestPaused(bool flag) {
    waitEndOfLastUpdate();
    writeState.isPaused = flag;
    isUpdated.store(true);
}

void OboeSourceSyncState::requestStopped(bool flag) {
    waitEndOfLastUpdate();
    writeState.isStopped = flag;
    isUpdated.store(true);
}

void OboeSourceSyncState::confirmStart() {
    assert(isEnded.load() == true && "Dobule start confirmation");
    isEnded.store(false);
}

void OboeSourceSyncState::confirmEnd() {
    assert(isEnded.load() == false && "Dobule end confirmation");
    isEnded.store(true);
}

bool OboeSourceSyncState::isEndConfirmed() const {
    return isEnded.load();
}

void OboeSourceSyncState::waitEndOfLastUpdate() {
    while(isUpdated.load() && !isEnded.load()) {
        std::this_thread::yield();
    }
}

void OboeSourceSyncState::syncRead() {
    if(!isUpdated.load()) {
        return;
    }
    readState = writeState;
    isUpdated.store(false);
}

const OboeSourceState& OboeSourceSyncState::getReadState() const {
    return readState;
}