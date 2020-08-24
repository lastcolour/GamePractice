#include "Audio/SoundEvent.hpp"
#include "Audio/ETSoundPlayManager.hpp"
#include "Core/ETPrimitives.hpp"
#include "Nodes/ETSoundNode.hpp"

#include <cassert>

SoundEvent::SoundEvent() :
    SoundEvent(InvalidEntityId) {
}

SoundEvent::SoundEvent(EntityId soundNodeId) :
    nodeId(soundNodeId) {
}

SoundEvent::SoundEvent(const SoundEvent& other) :
    nodeId(other.nodeId) {
}

SoundEvent& SoundEvent::operator=(const SoundEvent& other) {
    nodeId = other.nodeId;
    return *this;
}

SoundEvent::~SoundEvent() {
}

void SoundEvent::emit() {
    ET_QueueEvent(nodeId, &ETSoundEventNode::ET_emit);
}