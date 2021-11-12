#include "SoundData.hpp"

#include <cassert>

SoundData::SoundData() :
    fileName(""),
    loadState(LoadState::Released),
    useRefCount(0) {
}

SoundData::~SoundData() {
}

bool SoundData::isLoaded() const {
    return loadState.load() == LoadState::Loaded;
}

bool SoundData::isLoading() const {
    return loadState.load() == LoadState::Loading;
}

void SoundData::setLoading() {
    auto tReleasedState = LoadState::Released;
    if(!loadState.compare_exchange_strong(tReleasedState, LoadState::Loading)) {
        assert(false && "Invalid state");
        return;
    }
}

void SoundData::setLoaded(Memory::Buffer& buff) {
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

bool SoundData::hasUseRef() const {
    return useRefCount.load() > 0;
}