#include "SoundManager.hpp"
#include "Core/ETLogger.hpp"
#include "Core/ETAssets.hpp"
#include "OggDataStream.hpp"
#include "SoundImpl.hpp"
#include "SoundEventImpl.hpp"

#include <algorithm>

namespace {

const char* SOUNDS_ROOT_DIR = "Sounds";
const char* SOUND_EVENTS = "Sounds/Events.json";

} // namespace

SoundManager::SoundManager() {
}

SoundManager::~SoundManager() {
}

bool SoundManager::init() {
    if(!loadEvents()) {
        LogError("[SoundManager::init] Can't load sound events");
        return false;
    }
    ETNode<ETSoundManager>::connect(getEntityId());
    return true;
}

void SoundManager::deinit() {
}

Buffer SoundManager::loadSoundBuffer(const char* soundName) {
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

bool SoundManager::loadEvents() {
    JSONNode rootNode;
    ET_SendEventReturn(rootNode, &ETAssets::ET_loadJSONAsset, SOUND_EVENTS);
    if(!rootNode) {
        return false;
    }
    rootNode = rootNode.object("Events");
    if(!rootNode) {
        LogError("[SoundManager::loadEvents] Can't find required 'Events' node in '%s'", SOUND_EVENTS);
        return false;
    }
    for(auto& effectNode : rootNode) {
        std::string name;
        effectNode.read("name", name);
        if(name.empty()) {
            LogWarning("[SoundManager::loadEvents] Effect name empty");
            continue;
        }
        auto it = eventMap.find(name);
        if(it != eventMap.end()) {
            LogWarning("[SoundManager::loadEvents] Dublicate sound event '%s'", name);
            continue;
        }
        std::string sound;
        effectNode.read("sound", sound);
        if(sound.empty()) {
            LogWarning("[SoundManager::loadEvents] Event's '%s' sound is empty", name);
            continue;
        }
        auto buffer = loadSoundBuffer(sound.c_str());
        if(!buffer) {
            LogWarning("[SoundManager::loadEvents] Can't load event '%s' sound '%s'", name, sound);
            continue;
        }
        float volume = 1.f;
        effectNode.read("volume", volume);
        if(volume > 1.f || volume < 0.f) {
            LogWarning("[SoundManager::loadEvents] Event's '%s' volume should be in range: [0, 1]", name);
            volume = std::max(0.f, std::min(1.f, volume));
        }
        float nextDelay = 0.f;
        effectNode.read("nextDelay", nextDelay);
        if(nextDelay < 0.f) {
            LogWarning("[SoundManager::loadEvents] Event's '%s' nextDelay should be greater 0", name);
            nextDelay = std::max(nextDelay, 0.f);
        }

        auto event = std::shared_ptr<SoundEvent>(new SoundEventImpl(volume, nextDelay, buffer));
        eventMap[name] = event;
    }
    return true;
}

std::unique_ptr<Sound> SoundManager::ET_createSound(const char* soundName) {
    if(!soundName || !soundName[0]) {
        return nullptr;
    }
    auto buff = loadSoundBuffer(soundName);
    if(!buff) {
        return nullptr;
    }
    std::unique_ptr<Sound> sound(new SoundImpl(buff));
    return sound;
}

std::shared_ptr<SoundEvent> SoundManager::ET_createEvent(const char* eventName) {
    if(!eventName || !eventName[0]) {
        return nullptr;
    }
    auto it = eventMap.find(eventName);
    if(it == eventMap.end()) {
        LogWarning("[SoundManager::ET_createEvent] Can't find event: '%s'", eventName);
        return nullptr;
    }
    auto& event = it->second;
    return event;
}