#include "SoundDataManager.hpp"
#include "Core/ETAssets.hpp"

#include <cassert>

namespace {

void tryLoadSoundData(SoundData* soundData) {
    if(soundData->isLoading() || soundData->isLoaded()) {
        return;
    }
    soundData->setLoading();
    ET_SendEvent(&ETAsyncAssets::ET_asyncLoadAsset, soundData->fileName, [soundData](Buffer& buff){
        soundData->setLoaded(buff);
        ET_QueueEvent(&ETSoundDataManager::ET_updatePendingStarts);
    });
}

} // namespace

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

void SoundDataManager::ET_removeSoundProxy(SoundProxy* proxy) {
    ET_addSoundCmd(proxy, ESoundCommand::Stop, -1.f);

    std::lock_guard<std::mutex> lock(mutex);
    for(auto it = soundProxies.begin(); it != soundProxies.end(); ++it) {
        if((*it).get() == proxy) {
            soundProxies.erase(it);
            return;
        }
    }

    assert(false && "Can't find proxy to remove");
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

void SoundDataManager::ET_addSoundUpdate(std::function<void(void)> func) {
    func();
}

void SoundDataManager::ET_addSoundCmd(SoundProxy* proxyNode, ESoundCommand cmd, float duration) {
    switch(cmd) {
        case ESoundCommand::Start: {
            bool isEvent = false;
            tryStartSound(*proxyNode, duration, isEvent);
            break;
        }
        case ESoundCommand::Pause: {
            bool resetOffset = false;
            tryStopSound(*proxyNode, duration, resetOffset);
            break;
        }
        case ESoundCommand::Resume: {
            bool isEvent = false;
            tryStartSound(*proxyNode, duration, isEvent);
            break;
        }
        case ESoundCommand::Stop: {
            bool resetOffset = true;
            tryStopSound(*proxyNode, duration, resetOffset);
            break;
        }
        case ESoundCommand::Emit: {
            bool isEvent = true;
            tryStartSound(*proxyNode, duration, isEvent);
            break;
        }
        default: {
            assert(false && "Invalid sound cmd");
        }
    }
}

void SoundDataManager::ET_updatePendingStarts() {
    auto it = pendingStarts.begin();
    while(it != pendingStarts.end()) {
        auto proxy = it->first;
        auto startInfo = it->second;
        auto& soundData = proxy->getData();
        if(!soundData->isLoaded()) {
            ++it;
        } else {
            it = pendingStarts.erase(it);
            ET_addSoundCmd(proxy, ESoundCommand::Start, startInfo.duration);
        }
    }
}

void SoundDataManager::createPendingStart(SoundProxy& soundProxy, float duration) {
    auto it = pendingStarts.find(&soundProxy);
    if(it != pendingStarts.end()) {
        auto& startInfo = it->second;
        startInfo.duration = duration;
        return;
    }

    pendingStarts[&soundProxy] = PendingStartInfo{duration};

    auto& soundData = soundProxy.getData();
    tryLoadSoundData(soundData.get());
}

void SoundDataManager::removePendingStart(SoundProxy& soundProxy) {
    auto it = pendingStarts.find(&soundProxy);
    if(it != pendingStarts.end()) {
        pendingStarts.erase(it);
    }
}

void SoundDataManager::ET_updateSoundData() {
    auto it = sounds.begin();
    while(it != sounds.end()) {
        auto& soundData = it->second;
        if(soundData.use_count() == 1) {
            assert(!soundData->hasUseRef() && "Invalid use ref count");
            it = sounds.erase(it);
        } else {
            if(soundData->hasUseRef()) {
                tryLoadSoundData(soundData.get());
            } else if(soundData->tryFree()) {
                LogDebug("[SoundDataManager::updateSoundData] Unload sound data: '%s'", soundData->fileName);
            }
        }
        ++it;
    }
}

void SoundDataManager::tryStartSound(SoundProxy& soundProxy, float duration, bool isEvent) {
    if(soundProxy.isPlaying()) {
        return;
    }
    auto& soundData = soundProxy.getData();
    if(!soundData) {
        LogWarning("[SoundDataManager::tryStartSound] Can't start sound without set file");
        return;
    }
    if(!soundData->isLoaded()) {
        if(!isEvent) {
            createPendingStart(soundProxy, duration);
        } else {
            LogWarning("[SoundDataManager::tryStartSound] Can't start not loaded event");
        }
    } else if(soundData->data) {
        ET_SendEvent(&ETSoundPlayManager::ET_startSound, soundProxy, duration, isEvent);
    }
}

void SoundDataManager::tryStopSound(SoundProxy& soundProxy, float duration, bool resetOffset) {
    if(!soundProxy.isPlaying()) {
        if(resetOffset) {
            soundProxy.setOffset(0);
        }
        removePendingStart(soundProxy);
        return;
    }
    ET_SendEvent(&ETSoundPlayManager::ET_stopSound, soundProxy, duration, resetOffset);
}