#include "SoundProxy.hpp"
#include "Audio/ETSoundManagers.hpp"

#include <cassert>

SoundProxy::SoundProxy() :
    data(nullptr),
    mixNode(nullptr),
    offset(0),
    volume(1.f),
    group(ESoundGroup::Music),
    looped(false),
    hasSound(true),
    hasMixNode(false) {
}

SoundProxy::~SoundProxy() {
}

ESoundGroup SoundProxy::readGroup() const {
    return ESoundGroup::Game;
}

std::shared_ptr<SoundData>& SoundProxy::readData() {
    return data;
}

float SoundProxy::readVolume() const {
    return volume.load();
}

bool SoundProxy::readLooped() const {
    return looped.load();
}

void SoundProxy::writeFile(const char* fileName) {
    std::string currFile = readFile();
    if(currFile == fileName) {
        return;
    }
    if(isPlaying()) {
        stop();
    }
    std::string loadFileName = fileName;
    ET_QueueEvent(&ETSoundDataManager::ET_loadSoundData, this, loadFileName);
}

const char* SoundProxy::readFile() const {
    if(data) {
        return data->fileName;
    }
    return "";
}

void SoundProxy::writeData(std::shared_ptr<SoundData>& newData) {
    data = newData;
}

void SoundProxy::writeGroup(ESoundGroup newGroup) {
    group.store(newGroup);
    if(isPlaying()) {
        pause();
        ET_QueueEvent(&ETSoundPlayManager::ET_addSoundCmd, this, ESoundCommand::Resume);
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

void SoundProxy::play(const Sound& sound) {
    if(!isValid()) {
        LogWarning("[SoundProxy::play] Can't play source with invalid data: '%s'", readFile());
    } else {
        volume.store(sound.getVolume());
        looped.store(sound.isLooped());
        group.store(sound.getGroup());
        ET_QueueEvent(&ETSoundPlayManager::ET_addSoundCmd, this, ESoundCommand::Start);
    }
}

void SoundProxy::resume(const Sound& sound) {
    if(!isValid()) {
        LogWarning("[SoundProxy::play] Can't play source with invalid data: '%s'", readFile());
    } else {
        volume.store(sound.getVolume());
        looped.store(sound.isLooped());
        group.store(sound.getGroup());
        ET_QueueEvent(&ETSoundPlayManager::ET_addSoundCmd, this, ESoundCommand::Resume);
    }
}

void SoundProxy::pause() {
    ET_QueueEvent(&ETSoundPlayManager::ET_addSoundCmd, this, ESoundCommand::Pause);
}

void SoundProxy::stop() {
    ET_QueueEvent(&ETSoundPlayManager::ET_addSoundCmd, this, ESoundCommand::Stop);
}

bool SoundProxy::isPlaying() const {
    return hasMixNode.load();
}

bool SoundProxy::isValid() const {
    if(!data) {
        return true;
    }
    if(data->isLoaded.load()) {
        if(!data->data) {
            return false;
        }
    }
    return true;
}

bool SoundProxy::canRemove() const {
    if(hasSound.load()) {
        return false;
    }
    if(mixNode) {
        return false;
    }
    return true;
}