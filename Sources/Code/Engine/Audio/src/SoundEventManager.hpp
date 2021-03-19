#ifndef __SOUND_EVENT_MANAMGER_HPP__
#define __SOUND_EVENT_MANAMGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Audio/Sound.hpp"
#include "Audio/ETSoundManagers.hpp"
#include "Core/TimePoint.hpp"

class SoundEventInfo {
public:

    static void Reflect(ReflectContext& ctx);

public:

    SoundEventInfo();

public:

    Sound sound;
    std::string name;
    float nextDelay;
};

class SoundEventManager : public SystemLogic,
    public ETNode<ETSoundEventManager> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    SoundEventManager();
    virtual ~SoundEventManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETSoundEventManager
    void ET_emitEvent(const char* eventName) override;

private:

    struct EventData {
        SoundEventInfo* info;
        TimePoint lastPlayT;
    };

private:

    std::vector<SoundEventInfo> eventInfos;
    std::unordered_map<std::string, EventData> eventNodes;
};

#endif /* __SOUND_EVENT_MANAMGER_HPP__ */