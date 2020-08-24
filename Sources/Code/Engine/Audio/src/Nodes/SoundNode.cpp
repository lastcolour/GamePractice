#include "Nodes/SoundNode.hpp"
#include "MixGraph/SourceNode.hpp"
#include "Nodes/ETSoundNodeManager.hpp"
#include "Audio/ETSoundPlayManager.hpp"

#include <cassert>

SoundNode::SoundNode(EntityId soundNodeId) :
    nodeId(soundNodeId),
    mixNode(nullptr),
    volume(1.f),
    samplesOffset(0),
    looped(false) {

    assert(nodeId.isValid() && "Invalid node id");
    ETNode<ETSoundNode>::connect(nodeId);
}

SoundNode::~SoundNode() {
    ET_stop();
}

unsigned int SoundNode::getSamplesOffset() const {
    return samplesOffset;
}

void SoundNode::setMixNode(SourceNode* node) {
    if(mixNode) {
        assert(!node && "Can't change a mix node");
        samplesOffset = mixNode->getSamplesOffset();
        mixNode = nullptr;
    } else {
        assert(node && "Can't detach from a mix without attach");
        mixNode = node;
    }
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
    assert(soundData && "Invalid sound data");
    if(ET_isPlaying()) {
        return;
    }
    ET_SendEvent(&ETSoundPlayManager::ET_play, this);
}

void SoundNode::ET_stop() {
    if(mixNode) {
        samplesOffset = 0;
        mixNode->detachFromStream();
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
    volume = newVolume;
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

void SoundNode::setData(Buffer& data) {
    soundData = data;
}
