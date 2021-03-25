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
    hasSound(true),
    hasMixNode(false),
    isQueuedToStartMix(false) {
}

SoundProxy::~SoundProxy() {
}

ESoundGroup SoundProxy::readGroup() const {
    return group.load();
}

float SoundProxy::readVolume() const {
    return volume.load();
}

bool SoundProxy::readLooped() const {
    return looped.load();
}

void SoundProxy::setFile(const char* fileName) {
    std::string currFile = getFile();
    if(currFile == fileName) {
        return;
    }
    if(isPlaying()) {
        stop();
    }
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
    group.store(newGroup);
    if(isPlaying()) {
        pause();
        ET_QueueEvent(&ETSoundPlayManager::ET_addSoundCmd, this,
            ESoundCommand::Resume, INSTANT_DURATION);
    }
}

void SoundProxy::writeVolume(float newVolume) {
    volume.store(newVolume);
}

void SoundProxy::writeLooped(bool flag) {
    looped.store(flag);
}

void SoundProxy::setNoSound() {
    assert(hasSound.load() && "Already has no sound");
    if(isPlaying()) {
        stop();
    }
    hasSound.store(false);
}

void SoundProxy::setOffset(int newOffset) {
    offset = newOffset;
}

int SoundProxy::getOffset() const {
    return offset;
}

void SoundProxy::setMixNode(MixNode* node) {
    mixNode = node;
    if(mixNode) {
        hasMixNode.store(true);
    } else {
        hasMixNode.store(false);
    }
}

MixNode* SoundProxy::getMixNode() {
    return mixNode;
}

void SoundProxy::fadeInPlay(const Sound& sound, float duration) {
    if(!isValid()) {
        LogWarning("[SoundProxy::fadeInPlay] Can't play source with invalid data: '%s'", getFile());
    } else {
        volume.store(sound.getVolume());
        looped.store(sound.isLooped());
        group.store(sound.getGroup());
        ET_QueueEvent(&ETSoundPlayManager::ET_addSoundCmd, this,
            ESoundCommand::Start, duration);
    }
}

void SoundProxy::fadeOutStop(float duration) {
    ET_QueueEvent(&ETSoundPlayManager::ET_addSoundCmd, this,
        ESoundCommand::Stop, duration);
}

void SoundProxy::play(const Sound& sound) {
    if(!isValid()) {
        LogWarning("[SoundProxy::play] Can't play source with invalid data: '%s'", getFile());
    } else {
        volume.store(sound.getVolume());
        looped.store(sound.isLooped());
        group.store(sound.getGroup());
        ET_QueueEvent(&ETSoundPlayManager::ET_addSoundCmd, this,
            ESoundCommand::Start, INSTANT_DURATION);
    }
}

void SoundProxy::resume(const Sound& sound) {
    if(!isValid()) {
        LogWarning("[SoundProxy::play] Can't play source with invalid data: '%s'", getFile());
    } else {
        volume.store(sound.getVolume());
        looped.store(sound.isLooped());
        group.store(sound.getGroup());
        ET_QueueEvent(&ETSoundPlayManager::ET_addSoundCmd, this,
            ESoundCommand::Resume, INSTANT_DURATION);
    }
}

void SoundProxy::pause() {
    ET_QueueEvent(&ETSoundPlayManager::ET_addSoundCmd, this,
        ESoundCommand::Pause, INSTANT_DURATION);
}

void SoundProxy::stop() {
    ET_QueueEvent(&ETSoundPlayManager::ET_addSoundCmd, this, ESoundCommand::Stop,
        INSTANT_DURATION);
}

bool SoundProxy::isPlaying() const {
    return hasMixNode.load();
}

bool SoundProxy::shouldStartMix() const {
    return hasSound.load();
}

bool SoundProxy::isLoaded() const {
    if(soundData && soundData->isLoaded()) {
        return true;
    }
    return false;
}

void SoundProxy::setPendingStart(bool flag) {
    isQueuedToStartMix = flag;
}

bool SoundProxy::isPendingStart() const {
    return isQueuedToStartMix;
}

bool SoundProxy::isValid() const {
    if(!soundData) {
        return false;
    }
    return true;
}

bool SoundProxy::canRemove() const {
    if(hasSound.load()) {
        return false;
    }
    if(isQueuedToStartMix || mixNode) {
        return false;
    }
    return true;
}