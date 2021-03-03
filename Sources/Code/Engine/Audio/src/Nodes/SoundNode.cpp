#include "Nodes/SoundNode.hpp"
#include "MixGraph/SourceNode.hpp"
#include "Nodes/ETSoundNodeManager.hpp"
#include "Audio/ETSoundPlayManager.hpp"
#include "AudioUtils.hpp"

#include <cassert>

SoundNode::SoundNode(EntityId soundNodeId) :
    nodeId(soundNodeId),
    mixNode(nullptr),
    volume(1.f),
    samplesOffset(0),
    looped(false),
    dataIsSet(false) {

    assert(nodeId.isValid() && "Invalid node id");
    ETNode<ETSoundNode>::connect(nodeId);
}

SoundNode::~SoundNode() {
}

unsigned int SoundNode::getSamplesOffset() const {
    return samplesOffset;
}

void SoundNode::onAttachToMixNode(SourceNode* node) {
    assert(node && "Invalid mix node");
    assert(!mixNode && "Can't attach without detach");
    mixNode = node;
}

void SoundNode::onDetachFromMixNode(int newSampleOffset) {
    assert(mixNode && "Can't detach from a mix without attach");
    mixNode = nullptr;
    samplesOffset = newSampleOffset;
}

float SoundNode::getMixVolume() const {
    return volume;
}

bool SoundNode::isEvent() const {
    return false;
}

bool SoundNode::isMixLooped() const {
    return looped;
}

Buffer& SoundNode::getData() {
    return soundData;
}

void SoundNode::ET_play() {
    if(!soundData && !dataIsSet) {
        ET_QueueEvent(nodeId, &ETSoundNode::ET_play);
        return;
    }
    if(ET_isPlaying()) {
        return;
    }
    ET_SendEvent(&ETSoundPlayManager::ET_play, this);
}

void SoundNode::ET_stop() {
    if(mixNode) {
        mixNode->detachFromStream();
        samplesOffset = 0;
    }
}

void SoundNode::ET_pause() {
    if(mixNode) {
        mixNode->detachFromStream();
    }
}

void SoundNode::ET_resume() {
    if(!soundData) {
        return;
    }
    if(mixNode) {
        return;
    }
    ET_SendEvent(&ETSoundPlayManager::ET_play, this);
}

void SoundNode::ET_setLooped(bool flag) {
    looped = flag;
}

void SoundNode::ET_setVolume(float newVolume) {
    volume = Audio::ConvertToExpVolume(newVolume);
}

bool SoundNode::ET_isPlaying() const {
    return mixNode != nullptr;
}

bool SoundNode::ET_isPaused() const {
    if(mixNode) {
        return false;
    }
    return samplesOffset > 0;
}

EntityId SoundNode::getNodeId() const {
    return nodeId;
}

void SoundNode::setSoundData(Buffer& data) {
    soundData = data;
    dataIsSet = true;
}

ESoundGroup SoundNode::getGroup() const {
    return ESoundGroup::Music;
}