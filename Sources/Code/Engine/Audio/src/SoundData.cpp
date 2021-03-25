#include "SoundData.hpp"

#include <cassert>

SoundData::SoundData() :
    fileName(""),
    loadState(LoadState::Released),
    mixNodesRefCount(0),
    keepInMemory(false) {
}

SoundData::~SoundData() {
}

void SoundData::requestLoad() {
    assert(loadState == LoadState::Released && "Invalid load state");
    loadState = LoadState::LoadRequested;
}

bool SoundData::isLoaded() const {
    return loadState == LoadState::Loaded;
}

bool SoundData::isLoading() const {
    return loadState == LoadState::Loading;
}

bool SoundData::isLoadRequired() {
    return loadState == LoadState::LoadRequested;
}

void SoundData::setLoading() {
    assert(loadState == LoadState::LoadRequested && "Invalid load state");
    loadState = LoadState::Loading;
}

void SoundData::setLoaded(Buffer& buff) {
    assert(loadState == LoadState::Loading || loadState == LoadState::Released && "Invalid load state");
    data = std::move(buff);
    loadState = LoadState::Loaded;
}

bool SoundData::tryFree() {
    if(loadState != LoadState::Loaded) {
        return false;
    }
    if(mixNodesRefCount > 0 || keepInMemory) {
        return false;
    }
    loadState = LoadState::Released;
    data.reset();
    return true;
}

void SoundData::setKeepInMemory(bool flag) {
    keepInMemory = flag;
}

void SoundData::addMixNodeRef() {
    ++mixNodesRefCount;
}

void SoundData::removeMixNodeRef() {
    --mixNodesRefCount;
    assert(mixNodesRefCount >= 0 && "Invalid mix node ref count");
}