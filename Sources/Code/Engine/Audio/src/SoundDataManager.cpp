#include "SoundDataManager.hpp"
#include "Core/ETAssets.hpp"

SoundDataManager::SoundDataManager() {
}

SoundDataManager::~SoundDataManager() {
}

bool SoundDataManager::init() {
    ETNode<ETSoundDataManager>::connect(getEntityId());
    return true;
}

void SoundDataManager::deinit() {
}

SoundProxy* SoundDataManager::ET_createSoundProxy() {
    std::lock_guard<std::mutex> lock(mutex);
    soundProxies.emplace_back(new SoundProxy);
    return soundProxies.back().get();
}

void SoundDataManager::ET_updateData() {
    std::lock_guard<std::mutex> lock(mutex);
    updateProxies();
    updateSoundData();
}

void SoundDataManager::updateProxies() {
    auto it = soundProxies.begin();
    while(it != soundProxies.end()) {
        auto& soundProxy = *it;
        if(soundProxy->canRemove()) {
            it = soundProxies.erase(it);
        } else {
            soundProxy->updateDataLoadState();
            ++it;
        }
    }
}

void SoundDataManager::updateSoundData() {
    auto it = sounds.begin();
    while(it != sounds.end()) {
        auto& soundData = it->second;
        if(soundData.use_count() == 1) {
            it = sounds.erase(it);
            continue;
        }
        if(soundData->canStartLoading()) {
            ET_SendEvent(&ETAsyncAssets::ET_asyncLoadAsset, soundData->fileName, [soundDataPtr = soundData.get()](Buffer& buff){
                soundDataPtr->setLoaded(buff);
            });
        } else {
            if(soundData->tryFree()) {
                LogDebug("[SoundDataManager::updateSoundData] Unload sound data: '%s'", soundData->fileName);
            }
        }
        ++it;
    }
}

std::shared_ptr<SoundData> SoundDataManager::ET_createSoundData(const std::string& fileName) {
    if(fileName.empty()) {
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(mutex);

    auto it = sounds.find(fileName);
    if(it != sounds.end()) {
        return it->second;
    }

    auto res = sounds.emplace(fileName, new SoundData);

    it = res.first;
    auto& soundData = it->second;
    soundData->fileName = it->first.c_str();

    return soundData;
}