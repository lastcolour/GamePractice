#include "Particles/ParticlesUpdateFrameInfo.hpp"
#include "Render/ETParticlesSystem.hpp"

void DispatchParticlesSystemEvents(EntityId entId, ParticlesUpdateFrameInfo& updateInfo) {
    EParticlesEmitEventType flags = EParticlesEmitEventType::None;
    if(updateInfo.systemStarted) {
        flags = Core::EnumFlagsBitXORCreate(flags, EParticlesEmitEventType::OnSystemStart);
    }
    if(updateInfo.systemStopped) {
        flags = Core::EnumFlagsBitXORCreate(flags, EParticlesEmitEventType::OnSystemStop);
    }
    if(updateInfo.particlesSpawned) {
        flags = Core::EnumFlagsBitXORCreate(flags, EParticlesEmitEventType::OnParticleSpawn);
    }
    if(updateInfo.particlesRemoved) {
        flags = Core::EnumFlagsBitXORCreate(flags, EParticlesEmitEventType::OnParticleDeath);
    }
    if(flags == EParticlesEmitEventType::None) {
        return;
    }
    ET_SendEvent(entId, &ETParticlesSystemEvent::ET_onEmitEvent, flags);
}