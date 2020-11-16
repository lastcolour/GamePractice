#include "Logics/SoundEqualizerLogic.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Reflect/EnumInfo.hpp"
#include "Core/ETLogger.hpp"
#include "Entity/ETEntity.hpp"

void SoundEqualizerLogic::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<ESoundGroup>("ESoundGroup")) {
        enumInfo->addValues<ESoundGroup>({
            {"Game", ESoundGroup::Game},
            {"Music", ESoundGroup::Music},
            {"UI", ESoundGroup::UI}
        });
    }
    if(auto classInfo = ctx.classInfo<SoundEqualizerLogic>("SoundEqualizerLogic")) {
        classInfo->addField("group", &SoundEqualizerLogic::soundGroup);
        classInfo->addField("eqaulizer", &SoundEqualizerLogic::eqSetup);
        classInfo->addField("enabled", &SoundEqualizerLogic::autoEnable);
    }
}

SoundEqualizerLogic::SoundEqualizerLogic() :
    soundGroup(ESoundGroup::Game),
    autoEnable(false),
    isEnabled(false) {
}

SoundEqualizerLogic::~SoundEqualizerLogic() {
}

void SoundEqualizerLogic::ET_enable() {
    if(isEnabled) {
        return;
    }
    LogDebug("[SoundEqualizerLogic::ET_enable] Enable equalizer on entity: '%s'",
        EntityUtils::GetEntityName(getEntityId()));

    isEnabled = true;
    ET_QueueEvent(&ETAudioSystem::ET_setEqualizer, soundGroup, eqSetup);
}

void SoundEqualizerLogic::ET_disable() {
    if(!isEnabled) {
        return;
    }

    LogDebug("[SoundEqualizerLogic::ET_disable] Disable equalizer on entity: '%s'",
        EntityUtils::GetEntityName(getEntityId()));

    isEnabled = false;
    auto defSetup = EqualizerSetup();
    ET_QueueEvent(&ETAudioSystem::ET_setEqualizer, soundGroup, defSetup);
}

bool SoundEqualizerLogic::init() {
    ETNode<ETEqualizer>::connect(getEntityId());

    eqSetup.low = Math::Clamp(eqSetup.low, -1.f, 1.f);
    eqSetup.lowMid = Math::Clamp(eqSetup.lowMid, -1.f, 1.f);
    eqSetup.mid = Math::Clamp(eqSetup.mid, -1.f, 1.f);
    eqSetup.highMid = Math::Clamp(eqSetup.highMid, -1.f, 1.f);
    eqSetup.high = Math::Clamp(eqSetup.high, -1.f, 1.f);

    if(autoEnable) {
        ET_enable();
    }

    return true;
}

void SoundEqualizerLogic::deinit() {
    ET_disable();
}