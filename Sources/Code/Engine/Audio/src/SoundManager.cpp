#include "SoundManager.hpp"
#include "Core/ETLogger.hpp"
#include "Core/ETAssets.hpp"
#include "OggDataStream.hpp"
#include "Nodes/SoundNode.hpp"
#include "Nodes/SoundEventNode.hpp"

#include <algorithm>
#include <cassert>

namespace {

const char* SOUNDS_ROOT_DIR = "Sounds";
const char* SOUND_EVENTS = "Sounds/Events.json";

} // namespace

SoundManager::SoundManager() {
}

SoundManager::~SoundManager() {
}

bool SoundManager::init() {
    ETNode<ETSoundManager>::connect(getEntityId());
    ETNode<ETSoundNodeManager>::connect(getEntityId());
    ET_QueueEvent(&ETSoundNodeManager::ET_loadSoundEvents);
    return true;
}

void SoundManager::deinit() {
}

Buffer SoundManager::ET_loadSoundBuffer(const char* soundName) {
    auto it = buffers.find(soundName);
    if(it != buffers.end()) {
        return it->second;
    }

    Buffer buff;
    std::string reqSoundName = StringFormat("%s/%s", SOUNDS_ROOT_DIR, soundName);
    ET_SendEventReturn(buff, &ETAssets::ET_loadAsset, reqSoundName.c_str());

    if(buff) {
        buffers[soundName] = buff;
    }
    return buff;
}

void SoundManager::ET_loadSoundEvents() {
    JSONNode rootNode;
    ET_SendEventReturn(rootNode, &ETAssets::ET_loadJSONAsset, SOUND_EVENTS);
    if(!rootNode) {
        return;
    }
    rootNode = rootNode.object("Events");
    if(!rootNode) {
        LogError("[SoundManager::ET_loadSoundEvents] Can't find required 'Events' node in '%s'", SOUND_EVENTS);
        return;
    }
    for(auto& effectNode : rootNode) {
        std::string name;
        effectNode.read("name", name);
        if(name.empty()) {
            LogWarning("[SoundManager::ET_loadSoundEvents] Effect name empty");
            continue;
        }
        auto it = eventMap.find(name);
        if(it != eventMap.end()) {
            LogWarning("[SoundManager::ET_loadSoundEvents] Dublicate sound event '%s'", name);
            continue;
        }
        std::string sound;
        effectNode.read("sound", sound);
        if(sound.empty()) {
            LogWarning("[SoundManager::ET_loadSoundEvents] Event's '%s' sound is empty", name);
            continue;
        }
        auto buffer = ET_loadSoundBuffer(sound.c_str());
        if(!buffer) {
            LogWarning("[SoundManager::ET_loadSoundEvents] Can't load event '%s' sound '%s'", name, sound);
            continue;
        }
        float volume = 1.f;
        effectNode.read("volume", volume);
        if(volume > 1.f || volume < 0.f) {
            LogWarning("[SoundManager::ET_loadSoundEvents] Event's '%s' volume should be in range: [0, 1]", name);
            volume = std::max(0.f, std::min(1.f, volume));
        }
        float nextDelay = 0.f;
        effectNode.read("nextDelay", nextDelay);
        if(nextDelay < 0.f) {
            LogWarning("[SoundManager::ET_loadSoundEvents] Event's '%s' nextDelay should be greater 0", name);
            nextDelay = std::max(nextDelay, 0.f);
        }

        auto nodeId = GetETSystem()->createNewEntityId();
        std::unique_ptr<SoundEventNode> eventNode(new SoundEventNode(nodeId, buffer, volume, nextDelay));
        eventMap[name] = std::move(eventNode);
    }
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

void SoundManager::ET_initSoundNode(SoundNode* node, std::string soundName) {
    assert(node && "Invalid node");
    auto buffer = ET_loadSoundBuffer(soundName.c_str());
    if(!buffer) {
        LogError("[SoundManager::ET_createSoundNode] Can't load sound: '%s'", soundName);
        delete node;
        return;
    }
    node->setData(buffer);
    soundNodes[node->getNodeId()].reset(node);
}

SoundEvent SoundManager::ET_createEvent(const char* eventName) {
    if(!eventName || !eventName[0]) {
        return SoundEvent(InvalidEntityId);
    }
    bool isEventMapLoaded = false;
    auto it = eventMap.find(eventName);
    if(it == eventMap.end()) {
        LogWarning("[SoundManager::ET_createEvent] Can't find event: '%s'", eventName);
        return SoundEvent(InvalidEntityId);
    }
    auto& node = it->second;
    return SoundEvent(node->getNodeId());
}

void SoundManager::ET_removeSoundNode(EntityId nodeId) {
    auto it = soundNodes.find(nodeId);
    if(it != soundNodes.end()) {
        soundNodes.erase(it);
    }
}