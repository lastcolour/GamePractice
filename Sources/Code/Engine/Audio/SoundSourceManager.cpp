#include "Audio/SoundSourceManager.hpp"
#include "Audio/SoundSource.hpp"
#include "Core/ETLogger.hpp"

#include <cassert>

SoundSourceManager::SoundSourceManager() {
}

SoundSourceManager::~SoundSourceManager() {
}

bool SoundSourceManager::init() {
    ET_SendEventReturn(sources, &ETAudioSystem::ET_getSourcesToManage);
    if(sources.empty()) {
        LogError("[SoundSourceManager::init] Can't get sound sources to manage");
        return false;
    }
    sourceStateMap.resize(sources.size(), ESourceState::Free);
    ETNode<ETSystemTimerEvents>::connect(getEntityId());
    ETNode<ETSoundSourceManager>::connect(getEntityId());
    return true;
}

void SoundSourceManager::deinit() {
    ETNode<ETSystemTimerEvents>::disconnect();
    ETNode<ETSoundSourceManager>::disconnect();
}

SoundSource* SoundSourceManager::ET_getFreeSource() {
    for(size_t i = 0, sz = sourceStateMap.size(); i < sz; ++i) {
        if(sourceStateMap[i] == ESourceState::Free) {
            sourceStateMap[i] = ESourceState::Busy;
            return sources[i];
        }
    }
    return nullptr;
}

void SoundSourceManager::ET_returnSoundSource(SoundSource* retSoundSource) {
    assert(retSoundSource != nullptr && "Invalid sound source");
    for(size_t i = 0, sz = sources.size(); i < sz; ++i) {
        if(retSoundSource == sources[i]) {
            assert(sourceStateMap[i] == ESourceState::Busy && "Try return free source");
            sourceStateMap[i] = ESourceState::Free;
            return;
        }
    }
    assert(false && "Can't find sound source");
}

void SoundSourceManager::ET_onSystemTick(float dt) {
    for(size_t i = 0, sz = sources.size(); i < sz; ++i) {
        if(sourceStateMap[i] == ESourceState::Busy) {
            sources[i]->update();
        }
    }
}