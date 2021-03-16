#ifndef __SOUND_EVENT_TABLE_HPP__
#define __SOUND_EVENT_TABLE_HPP__

#include "Audio/ETAudioSystem.hpp"
#include "Nodes/SoundEventNode.hpp"

class SoundEventInfo {
public:

    static void Reflect(ReflectContext& ctx);

public:

    SoundEventInfo();

public:

    std::string name;
    std::string sound;
    ESoundGroup group;
    float volume;
    float nextDelay;
    Buffer data;
};

class SoundEventTable {
public:

    static void Reflect(ReflectContext& ctx);

public:

    SoundEventTable();
    ~SoundEventTable();

    void load();
    EntityId findEventNodeId(const std::string& eventName);
    std::vector<SoundEventInfo>& getEventsInfo();

private:

    std::vector<SoundEventInfo> eventsInfo;
    std::unordered_map<std::string, std::unique_ptr<SoundEventNode>> eventNodes;
};

#endif /* __SOUND_EVENT_TABLE_HPP__ */