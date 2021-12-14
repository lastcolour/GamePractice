#include "Particles/ParticlesUpdateFrameInfo.hpp"
#include "Render/ETParticlesSystem.hpp"

void DispatchParticlesSystemEvents(EntityId entId, ParticlesUpdateFrameInfo& updateInfo) {
    EParticlesEmitEventType flags = EParticlesEmitEventType::None;
    if(updateInfo.systemStarted) {
        flags = Core::EnumFlagsBitXOR(flags, EParticlesEmitEventType::OnSystemStart);
    }
    if(updateInfo.systemStopped) {
        flags = Core::EnumFlagsBitXOR(flags, EParticlesEmitEventType::OnSystemStop);
    }
    if(updateInfo.particlesSpawned) {
        flags = Core::EnumFlagsBitXOR(flags, EParticlesEmitEventType::OnParticleSpawn);
    }
    if(updateInfo.particlesRemoved) {
        flags = Core::EnumFlagsBitXOR(flags, EParticlesEmitEventType::OnParticleDeath);
    }
    if(flags == EParticlesEmitEventType::None) {
        return;
    }
    ET_SendEvent(entId, &ETParticlesSystemEvent::ET_onEmitEvent, flags);
}