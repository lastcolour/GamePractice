#include "SoundData.hpp"

#include <cassert>

SoundData::SoundData() :
    fileName(""),
    loadState(LoadState::Released),
    useRefCount(0) {
}

SoundData::~SoundData() {
}

void SoundData::requestLoad() {
    auto tReleaseState = LoadState::Released;
    loadState.compare_exchange_strong(tReleaseState, LoadState::LoadRequested);
}

bool SoundData::isLoaded() const {
    return loadState.load() == LoadState::Loaded;
}

bool SoundData::canStartLoading() {
    auto tRequestState = LoadState::LoadRequested;
    return loadState.compare_exchange_strong(tRequestState, LoadState::Loading);
}

void SoundData::setLoaded(Buffer& buff) {
    auto tLoadingState = LoadState::Loading;
    if(!loadState.compare_exchange_strong(tLoadingState, LoadState::Loaded)) {
        assert(false && "Invalid state");
        return;
    }
    data = std::move(buff);
}

bool SoundData::tryFree() {
    if(useRefCount.load() > 0) {
        return false;
    }
    auto tLoadedState = LoadState::Loaded;
    if(!loadState.compare_exchange_strong(tLoadedState, LoadState::Released)) {
        return false;
    }
    data.reset();
    return true;
}

void SoundData::addUseRef() {
    useRefCount.fetch_add(1);
}

void SoundData::removeUseRef() {
    useRefCount.fetch_sub(1);
    assert(useRefCount.load() >= 0 && "Invalid use ref count");
}