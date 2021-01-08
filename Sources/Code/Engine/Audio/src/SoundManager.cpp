#include "SoundManager.hpp"
#include "Core/ETLogger.hpp"
#include "Nodes/SoundNode.hpp"

#include <cassert>

SoundManager::SoundManager() {
}

SoundManager::~SoundManager() {
}

bool SoundManager::init() {
    ETNode<ETSoundManager>::connect(getEntityId());
    ETNode<ETSoundNodeManager>::connect(getEntityId());

    eventTable.load();
    ET_QueueEvent(&ETSoundNodeManager::ET_loadSoundEventsBuffers);

    return true;
}

void SoundManager::deinit() {
}

void SoundManager::ET_loadSoundEventsBuffers() {
    bufferManager.loadEvents(eventTable);
}

Sound SoundManager::ET_createSound(const char* soundName) {
    std::string name = soundName;
    if(name.empty()) {
        return Sound(InvalidEntityId);
    }
    auto node = new SoundNode(GetETSystem()->createNewEntityId());
    ET_QueueEvent(&ETSoundNodeManager::ET_initSoundNode, node, name);
    return Sound(node->getNodeId());
}

SoundEvent SoundManager::ET_createEvent(const char* eventName) {
    auto nodeId = eventTable.findEventNodeId(eventName);
    return SoundEvent(nodeId);
}

void SoundManager::ET_initSoundNode(SoundNode* node, std::string soundName) {
    assert(node && "Invalid node");
    auto buffer = bufferManager.loadSound(soundName);
    if(!buffer) {
        LogError("[SoundManager::ET_createSoundNode] Can't load sound: '%s'", soundName);
        delete node;
        return;
    }
    node->setSoundData(buffer);
    soundNodes[node->getNodeId()].reset(node);
}

void SoundManager::ET_removeSoundNode(EntityId nodeId) {
    auto it = soundNodes.find(nodeId);
    if(it != soundNodes.end()) {
        it->second->ET_stop();
        soundNodes.erase(it);
    }
}