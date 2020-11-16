#include "Nodes/SoundEventNode.hpp"
#include "Audio/ETSoundPlayManager.hpp"
#include "SoundEventTable.hpp"

#include <cassert>


SoundEventNode::SoundEventNode(EntityId soundNodeId, SoundEventInfo* eventInfo) :
    nodeId(soundNodeId),
    info(eventInfo) {

    assert(nodeId.isValid() && "Invalid node id");
    assert(info && "invalid event info");

    ETNode<ETSoundEventNode>::connect(nodeId);
}

SoundEventNode::~SoundEventNode() {
}

unsigned int SoundEventNode::getSamplesOffset() const {
    return 0u;
}

void SoundEventNode::onAttachToMixNode(SourceNode* node) {
}

void SoundEventNode::onDetachFromMixNode(int newSampleOffset) {
}

float SoundEventNode::getMixVolume() const {
    return info->volume;
}

bool SoundEventNode::isEvent() const {
    return true;
}

bool SoundEventNode::isMixLooped() const {
    return false;
}

Buffer& SoundEventNode::getData() {
    return info->data;
}

void SoundEventNode::ET_emit() {
    if(!info->data) {
        return;
    }
    TimePoint frameStartT = TimePoint::GetNowTime();
    if(frameStartT.getSecElapsedFrom(lastPlayTime) < (info->nextDelay + 0.001f)) {
        return;
    }
    lastPlayTime = frameStartT;
    ET_SendEvent(&ETSoundPlayManager::ET_play, this);
}

EntityId SoundEventNode::getNodeId() const {
    return nodeId;
}

ESoundGroup SoundEventNode::getGroup() const {
    return info->group;
}