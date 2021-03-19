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
    return nullptr;
}

void SoundDataManager::ET_removeSoundProxy(SoundProxy* state) {
}

std::shared_ptr<SoundData> SoundDataManager::ET_loadSoundData(const char* fileName) {
    if(!fileName || !fileName[0]) {
        return nullptr;
    }

    auto it = sounds.find(fileName);
    if(it != sounds.end()) {
        return it->second;
    }

    Buffer buff;
    ET_SendEventReturn(buff, &ETAssets::ET_loadAsset, fileName);
    if(!buff) {
        return nullptr;
    }

    auto& soundData = sounds[fileName];
    soundData.reset(new SoundData());
    soundData->data = std::move(buff);
    soundData->fileName = fileName;

    return soundData;
}