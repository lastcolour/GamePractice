#include "SoundProxy.hpp"
#include "Audio/ETSoundManagers.hpp"

#include <cassert>

namespace {

const float INSTANT_DURATION = -1.f;

} // namespace

SoundProxy::SoundProxy() :
    soundData(nullptr),
    mixNode(nullptr),
    offset(0),
    volume(1.f),
    group(ESoundGroup::Music),
    looped(false),
    keepInMemory(false) {
}

SoundProxy::~SoundProxy() {
    setKeepLoaded(false);
}

ESoundGroup SoundProxy::readGroup() const {
    return group;
}

float SoundProxy::readVolume() const {
    return volume;
}

bool SoundProxy::readLooped() const {
    return looped;
}

void SoundProxy::setFile(const char* fileName) {
    std::string currFile = getFile();
    if(currFile == fileName) {
        return;
    }
    if(isPlaying()) {
        stop();
    }
    setKeepLoaded(false);
    ET_SendEventReturn(soundData, &ETSoundDataManager::ET_createSoundData, fileName);
}

const char* SoundProxy::getFile() const {
    if(soundData) {
        return soundData->fileName;
    }
    return "";
}

std::shared_ptr<SoundData>& SoundProxy::getData() {
    return soundData;
}

void SoundProxy::setData(std::shared_ptr<SoundData>& newSoundData) {
    soundData = newSoundData;
}

void SoundProxy::writeGroup(ESoundGroup newGroup) {
    ET_QueueEvent(&ETSoundDataManager::ET_addSoundUpdate, [this, newGroup](){
        group = newGroup;
    });

    if(isPlaying()) {
        pause();
        ET_QueueEvent(&ETSoundDataManager::ET_addSoundCmd, this,
            ESoundCommand::Resume, INSTANT_DURATION);
    }
}

void SoundProxy::writeVolume(float newVolume) {
    ET_QueueEvent(&ETSoundDataManager::ET_addSoundUpdate, [this, newVolume](){
        volume = newVolume;
    });
}

void SoundProxy::writeLooped(bool flag) {
    ET_QueueEvent(&ETSoundDataManager::ET_addSoundUpdate, [this, flag](){
        looped = flag;
    });
}

void SoundProxy::setNoSound() {
    ET_QueueEvent(&ETSoundDataManager::ET_removeSoundProxy, this);
}

void SoundProxy::setOffset(int newOffset) {
    offset = newOffset;
}

int SoundProxy::getOffset() const {
    return offset;
}

void SoundProxy::setMixNode(MixNode* node) {
    mixNode.store(node);
}

MixNode* SoundProxy::getMixNode() {
    return mixNode.load();
}

void SoundProxy::fadeInPlay(const Sound& sound, float duration) {
    syncPlayParams(sound);
    ET_QueueEvent(&ETSoundDataManager::ET_addSoundCmd, this,
        ESoundCommand::Start, duration);
}

void SoundProxy::fadeOutStop(float duration) {
    ET_QueueEvent(&ETSoundDataManager::ET_addSoundCmd, this,
        ESoundCommand::Stop, duration);
}

void SoundProxy::play(const Sound& sound) {
    syncPlayParams(sound);
    ET_QueueEvent(&ETSoundDataManager::ET_addSoundCmd, this,
        ESoundCommand::Start, INSTANT_DURATION);
}

void SoundProxy::resume(const Sound& sound) {
    syncPlayParams(sound);
    ET_QueueEvent(&ETSoundDataManager::ET_addSoundCmd, this,
        ESoundCommand::Resume, INSTANT_DURATION);
}

void SoundProxy::pause() {
    ET_QueueEvent(&ETSoundDataManager::ET_addSoundCmd, this,
        ESoundCommand::Pause, INSTANT_DURATION);
}

void SoundProxy::stop() {
    ET_QueueEvent(&ETSoundDataManager::ET_addSoundCmd, this, ESoundCommand::Stop,
        INSTANT_DURATION);
}

bool SoundProxy::isPlaying() const {
    return mixNode.load() != nullptr;
}

void SoundProxy::setKeepLoaded(bool flag) {
    auto revFlag = !flag;
    if(!keepInMemory.compare_exchange_strong(revFlag, flag)) {
        return;
    }
    if(!soundData) {
        return;
    }
    if(flag) {
        soundData->addUseRef();
    } else {
        soundData->removeUseRef();
    }
}

void SoundProxy::syncPlayParams(const Sound& sound) {
    auto newVolume = sound.getVolume();
    auto newLooped = sound.isLooped();
    auto newGroup = sound.getGroup();
    ET_QueueEvent(&ETSoundDataManager::ET_addSoundUpdate, [this, newVolume, newLooped, newGroup](){
        volume = newVolume;
        looped = newLooped;
        group = newGroup;
    });
}

void SoundProxy::emit() {
    ET_QueueEvent(&ETSoundDataManager::ET_addSoundCmd, this,
        ESoundCommand::Emit, INSTANT_DURATION);
}