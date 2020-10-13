#include "Nodes/SoundEventNode.hpp"
#include "Audio/ETSoundPlayManager.hpp"

#include <cassert>

SoundEventNode::SoundEventNode(EntityId soundNodeId, const std::string& soundFileName, float eventVolume, float eventDelay) :
    nodeId(soundNodeId),
    soundName(soundFileName),
    volume(eventVolume),
    nextDelay(eventDelay) {

    assert(nodeId.isValid() && "Invalid node id");

    ETNode<ETSoundEventNode>::connect(nodeId);
}

const std::string& SoundEventNode::getSoundName() const {
    return soundName;
}

void SoundEventNode::setSoundData(Buffer& buffer) {
    assert(buffer && "Invalid sound data");
    soundData = buffer;
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
    if(!soundData) {
        return;
    }
    TimePoint frameStartT = TimePoint::GetNowTime();
    if(frameStartT.getSecElapsedFrom(lastPlayTime) < (nextDelay + 0.001f)) {
        return;
    }
    lastPlayTime = frameStartT;
    ET_SendEvent(&ETSoundPlayManager::ET_play, this);
}

EntityId SoundEventNode::getNodeId() const {
    return nodeId;
}
