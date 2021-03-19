#include "SoundEventManager.hpp"
#include "Reflect/ReflectUtils.hpp"

namespace {

const char* SOUND_EVENT_TABLE = "Sounds/Events.json";

} // namespace

void SoundEventInfo::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<SoundEventInfo>("SoundEventInfo")) {
        classInfo->addField("sound", &SoundEventInfo::sound);
        classInfo->addField("name", &SoundEventInfo::name);
        classInfo->addField("nextDelay", &SoundEventInfo::nextDelay);
    }
}

SoundEventInfo::SoundEventInfo() :
    nextDelay(0.f) {
}

void SoundEventManager::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<SoundEventManager>("SoundEventManager")) {
        classInfo->addField("events", &SoundEventManager::eventInfos);
    }
}

SoundEventManager::SoundEventManager() {
}

SoundEventManager::~SoundEventManager() {
}

bool SoundEventManager::init() {
    if(!ReflectUtils::LoadObjectFromAsset(*this, SOUND_EVENT_TABLE)) {
        LogError("[SoundEventManager::init] Can't load sound events from: '%s'", SOUND_EVENT_TABLE);
        return true;
    }

    for(auto& info : eventInfos) {
        if(info.name.empty()) {
            LogWarning("[SoundEventManager::init] Empty event name");
            continue;
        }
        if(!info.sound.isValid()) {
            LogWarning("[SoundEventManager::init] Invalid sound for event: '%s'", info.name);
            continue;
        }
        auto it = eventNodes.find(info.name);
        if(it != eventNodes.end()) {
            LogWarning("[SoundEventManager::init] Duplicate event: '%s'", info.name);
            continue;
        }
        auto& data = eventNodes[info.name];
        data.info = &info;
    }

    ETNode<ETSoundEventManager>::connect(getEntityId());
    return true;
}

void SoundEventManager::deinit() {
}

void SoundEventManager::ET_emitEvent(const char* eventName) {
    auto it = eventNodes.find(eventName);
    if(it == eventNodes.end()) {
        LogWarning("[SoundEventManager::ET_emitEvent] Can't find event data: '%s'", eventName);
        return;
    }
    auto& data = it->second;
    auto currTime = TimePoint::GetNowTime();
    if(currTime.getSecElapsedFrom(data.lastPlayT) < (data.info->nextDelay + 0.001f)) {
        return;
    }
    data.info->sound.play();
}