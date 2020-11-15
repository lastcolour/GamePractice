#include "Logics/SoundEqualizerLogic.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Reflect/EnumInfo.hpp"

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
    }
}

SoundEqualizerLogic::SoundEqualizerLogic() :
    soundGroup(ESoundGroup::Game) {
}

SoundEqualizerLogic::~SoundEqualizerLogic() {
}

void SoundEqualizerLogic::ET_enable() {
}

void SoundEqualizerLogic::ET_disable() {
}

bool SoundEqualizerLogic::init() {
    ETNode<ETEqualizer>::connect(getEntityId());

    eqSetup.low = Math::Clamp(eqSetup.low, -1.f, 1.f);
    eqSetup.lowMid = Math::Clamp(eqSetup.lowMid, -1.f, 1.f);
    eqSetup.mid = Math::Clamp(eqSetup.mid, -1.f, 1.f);
    eqSetup.highMid = Math::Clamp(eqSetup.highMid, -1.f, 1.f);
    eqSetup.high = Math::Clamp(eqSetup.high, -1.f, 1.f);

    ET_QueueEvent(&ETAudioSystem::ET_setEqualizer, soundGroup, eqSetup);

    return true;
}

void SoundEqualizerLogic::deinit() {
}