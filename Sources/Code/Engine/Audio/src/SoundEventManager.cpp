#include "SoundEventManager.hpp"
#include "Reflect/ReflectUtils.hpp"
#include "SoundProxy.hpp"
#include "Core/GlobalData.hpp"
#include "SoundConfig.hpp"

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
    auto& soundEventTable = GetGlobal<SoundConfig>()->soundEventTable;
    if(soundEventTable.empty()) {
        LogError("[SoundEventManager::init] Sound event table not specified");
        return false;
    }
    if(!ReflectUtils::LoadObjectFromAsset(*this, soundEventTable.c_str())) {
        LogError("[SoundEventManager::init] Can't load sound events from: '%s'", soundEventTable);
        return true;
    }

    for(auto& info : eventInfos) {
        if(info.name.empty()) {
            LogWarning("[SoundEventManager::init] Empty event name");
            continue;
        }
        auto it = eventNodes.find(info.name);
        if(it != eventNodes.end()) {
            LogWarning("[SoundEventManager::init] Duplicate event: '%s'", info.name);
            continue;
        }
        auto& data = eventNodes[info.name];
        info.sound.setLooped(false);
        info.sound.setKeepLoaded(true);
        data.info = &info;

        auto proxy = data.info->sound.getProxy();
        proxy->syncPlayParams(info.sound);
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
    data.lastPlayT = currTime;
    auto proxy = data.info->sound.getProxy();
    proxy->emit();
}