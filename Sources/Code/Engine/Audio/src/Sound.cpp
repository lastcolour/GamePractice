#include "Audio/Sound.hpp"
#include "Nodes/ETSoundNode.hpp"
#include "Nodes/ETSoundNodeManager.hpp"

Sound::Sound() :
    Sound(InvalidEntityId) {
}

Sound::Sound(EntityId soundNodeId) :
    nodeId(soundNodeId),
    volume(1.f),
    looped(false) {
}

Sound::Sound(Sound&& other) :
    nodeId(other.nodeId),
    volume(other.volume),
    looped(other.looped) {

    other.nodeId = InvalidEntityId;
}

Sound& Sound::operator=(Sound&& other) {
    if(this != &other) {
        stop();
        nodeId = other.nodeId;
        volume = other.volume;
        looped = other.looped;

        other.nodeId = InvalidEntityId;
    }
    return *this;
}

Sound::~Sound() {
    ET_QueueEvent(&ETSoundNodeManager::ET_removeSoundNode, nodeId);
}

void Sound::play() {
    ET_QueueEvent(nodeId, &ETSoundNode::ET_play);
}

void Sound::stop() {
    ET_QueueEvent(nodeId, &ETSoundNode::ET_stop);
}

void Sound::pause() {
    ET_QueueEvent(nodeId, &ETSoundNode::ET_pause);
}

void Sound::resume() {
    ET_QueueEvent(nodeId, &ETSoundNode::ET_resume);
}

void Sound::setLooped(bool flag) {
    looped = flag;
    ET_QueueEvent(nodeId, &ETSoundNode::ET_setLooped, looped);
}

bool Sound::isLooped() const {
    return looped;
}

void Sound::setVolume(float newVolume) {
    volume = newVolume;
    ET_QueueEvent(nodeId, &ETSoundNode::ET_setVolume, volume);
}

float Sound::getVolume() const {
    return volume;
}

bool Sound::isPlaying() const {
    bool res = false;
    ET_SendEventReturn(res, nodeId, &ETSoundNode::ET_isPlaying);
    return res;
}

bool Sound::isPaused() const {
    bool res = false;
    ET_SendEventReturn(res, nodeId, &ETSoundNode::ET_isPaused);
    return res;
}

bool Sound::isValid() const {
    return nodeId.isValid();
}