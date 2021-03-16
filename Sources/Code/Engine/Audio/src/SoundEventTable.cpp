#include "SoundEventTable.hpp"
#include "Reflect/ReflectUtils.hpp"
#include "AudioUtils.hpp"

namespace {

const char* SOUND_EVENT_TABLE = "Sounds/Events.json";

} // namespace

SoundEventInfo::SoundEventInfo() :
    group(ESoundGroup::Game),
    volume(1.f),
    nextDelay(0.f) {
}

void SoundEventInfo::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<SoundEventInfo>("SoundEventInfo")) {
        classInfo->addResourceField("sound", ResourceType::Sound, &SoundEventInfo::sound);
        classInfo->addField("name", &SoundEventInfo::name);
        classInfo->addField("nextDelay", &SoundEventInfo::nextDelay);
        classInfo->addField("volume", &SoundEventInfo::volume);
        classInfo->addField("group", &SoundEventInfo::group);
    }
}

void SoundEventTable::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<SoundEventTable>("SoundEventTable")) {
        classInfo->addField("events", &SoundEventTable::eventsInfo);
    }
}

SoundEventTable::SoundEventTable() {
}

SoundEventTable::~SoundEventTable() {
}

EntityId SoundEventTable::findEventNodeId(const std::string& eventName) {
    if(eventName.empty()) {
        return InvalidEntityId;
    }
    auto it = eventNodes.find(eventName);
    if(it == eventNodes.end()) {
        LogWarning("[SoundManager::findEventNodeId] Can't find event with name: '%s'", eventName);
        return InvalidEntityId;
    }
    auto& node = it->second;
    return node->getNodeId();
}

void SoundEventTable::load() {
    eventNodes.clear();
    eventsInfo.clear();
    if(!ReflectUtils::LoadObjectFromAsset(*this, SOUND_EVENT_TABLE)) {
        return;
    }
    for(auto& info : eventsInfo) {
        info.volume = Audio::ConvertToExpVolume(info.volume);
        std::unique_ptr<SoundEventNode> eventNode(
            new SoundEventNode(GetETSystem()->createNewEntityId(), &info));
        eventNodes[info.name] = std::move(eventNode);
    }
}

std::vector<SoundEventInfo>& SoundEventTable::getEventsInfo() {
    return eventsInfo;
}