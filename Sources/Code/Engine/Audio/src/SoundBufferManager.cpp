#include "SoundBufferManager.hpp"
#include "Core/ETAssets.hpp"
#include "Core/ETLogger.hpp"
#include "SoundEventTable.hpp"

SoundBufferManager::SoundBufferManager() {
}

SoundBufferManager::~SoundBufferManager() {
}

void SoundBufferManager::loadEvents(SoundEventTable& eventTable) {
    auto& eventsInfo = eventTable.getEventsInfo();
    for(auto& event : eventsInfo) {
        auto buffer = loadSound(event.sound);
        if(!buffer) {
            LogWarning("[SoundManager::loadEvents] Can't load event's '%s' sound: '%s'", event.name, event.sound);
            continue;
        }
        event.data = buffer;
    }
}

Buffer SoundBufferManager::loadSound(const std::string& fileName) {
    auto it = buffers.find(fileName.c_str());
    if(it != buffers.end()) {
        return it->second;
    }

    Buffer buff;
    ET_SendEventReturn(buff, &ETAssets::ET_loadAsset, fileName.c_str());

    if(buff) {
        buffers[fileName.c_str()] = buff;
    }
    return buff;
}