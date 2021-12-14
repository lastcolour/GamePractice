#include "Logics/ParticlesSoundEvent.hpp"
#include "Reflect/EnumInfo.hpp"

void ParticlesSoundEvent::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<EParticlesEmitEventType>("EParticlesEmitEventType")) {
        enumInfo->addValues<EParticlesEmitEventType>({
            {"OnSystemStart", EParticlesEmitEventType::OnSystemStart},
            {"OnSystemStop", EParticlesEmitEventType::OnSystemStop},
            {"OnParticleSpawn", EParticlesEmitEventType::OnParticleSpawn},
            {"OnParticleDeath", EParticlesEmitEventType::OnParticleDeath}
        });
    }
    if(auto classInfo = ctx.classInfo<ParticlesSoundEvent>("ParticlesSoundEvent")) {
        classInfo->addField("emitType", &ParticlesSoundEvent::emitType);
        classInfo->addField("soundEvent", &ParticlesSoundEvent::soundEvent);
    }
}

ParticlesSoundEvent::ParticlesSoundEvent() :
    emitType(EParticlesEmitEventType::OnSystemStart) {
}

ParticlesSoundEvent::~ParticlesSoundEvent() {
}

void ParticlesSoundEvent::init() {
    ETNode<ETParticlesSystemEvent>::connect(getEntityId());
}

void ParticlesSoundEvent::deinit() {
}

void ParticlesSoundEvent::ET_onEmitEvent(EParticlesEmitEventType emitFlags) {
    if(Core::EnumFlagsBitAND(emitType, emitFlags)) {
        soundEvent.emit();
    }
}