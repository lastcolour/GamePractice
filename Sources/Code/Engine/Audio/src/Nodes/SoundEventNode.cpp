#include "Nodes/SoundEventNode.hpp"
#include "Audio/ETSoundPlayManager.hpp"

#include <cassert>

SoundEventNode::SoundEventNode(EntityId soundNodeId, Buffer& data, float eventVolume, float eventDelay) :
    nodeId(soundNodeId),
    soundData(data),
    volume(eventVolume),
    nextDelay(eventDelay) {

    assert(nodeId.isValid() && "Invalid node id");
    assert(soundData && "Invalid sound data");

    ETNode<ETSoundEventNode>::connect(nodeId);
}

SoundEventNode::~SoundEventNode() {
}

unsigned int SoundEventNode::getSamplesOffset() const {
    return 0u;
}

void SoundEventNode::setMixNode(SourceNode* node) {
}

float SoundEventNode::getMixVolume() const {
    return volume;
}

bool SoundEventNode::isEvent() const {
    return true;
}

bool SoundEventNode::isMixLooped() const {
    return false;
}

Buffer& SoundEventNode::getData() {
    return soundData;
}

void SoundEventNode::ET_emit() {
    TimePoint frameStartT = TimePoint::GetNowTime();
    if(frameStartT.getSecondsElapsedFrom(lastPlayTime) < (nextDelay + 0.001f)) {
        return;
    }
    lastPlayTime = frameStartT;
    ET_SendEvent(&ETSoundPlayManager::ET_play, this);
}

EntityId SoundEventNode::getNodeId() const {
    return nodeId;
}
