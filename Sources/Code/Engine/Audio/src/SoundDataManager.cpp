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

void SoundDataManager::ET_loadSoundData(SoundProxy* soundProxy, const std::string& fileName) {
    if(fileName.empty()) {
        return;
    }
    if(soundProxy->canRemove()) {
        return;
    }

    auto it = sounds.find(fileName);
    if(it != sounds.end()) {
        soundProxy->writeData(it->second);
        return;
    }

    auto& soundData = sounds[fileName];
    soundData.reset(new SoundData());
    soundData->fileName = fileName.c_str();
    soundData->isLoaded.store(false);

    soundProxy->writeData(soundData);

    ET_SendEvent(&ETAsyncAssets::ET_asyncLoadAsset, fileName.c_str(), [soundData](Buffer& buff){
        soundData->data = std::move(buff);
        soundData->isLoaded.store(true);
    });
}