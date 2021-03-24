#include "SoundDataManager.hpp"
#include "Core/ETAssets.hpp"

SoundDataManager::SoundDataManager() {
}

SoundDataManager::~SoundDataManager() {
}

bool SoundDataManager::init() {
    nullSoundData.reset(new SoundData);
    nullSoundData->isLoaded.store(true);

    ETNode<ETSoundDataManager>::connect(getEntityId());
    return true;
}

void SoundDataManager::deinit() {
}

SoundProxy* SoundDataManager::ET_createSoundProxy() {
    soundProxies.emplace_back(new SoundProxy);
    return soundProxies.back().get();
}

void SoundDataManager::ET_cleanUpData() {
    auto it = soundProxies.begin();
    while(it != soundProxies.end()) {
        if((*it)->canRemove()) {
            it = soundProxies.erase(it);
        } else {
            ++it;
        }
    }
}

void SoundDataManager::ET_setupSoundData(SoundProxy* soundProxy, const std::string& fileName) {
    if(soundProxy->canRemove()) {
        return;
    }
    if(fileName.empty()) {
        soundProxy->setData(nullSoundData);
        return;
    }

    auto it = sounds.find(fileName);
    if(it != sounds.end()) {
        soundProxy->setData(it->second);
        return;
    }

    auto res = sounds.emplace(fileName, new SoundData);

    it = res.first;
    auto& soundData = it->second;
    soundData->fileName = it->first.c_str();
    soundData->isLoaded.store(false);

    soundProxy->setData(soundData);

    ET_SendEvent(&ETAsyncAssets::ET_asyncLoadAsset, fileName.c_str(), [soundData](Buffer& buff){
        soundData->data = std::move(buff);
        soundData->isLoaded.store(true);
    });
}