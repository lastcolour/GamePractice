#include "Particles/ParticlesUpdateFrameInfo.hpp"
#include "Core/Utils.hpp"
#include "Render/ETParticlesSystem.hpp"

void DispatchParticlesSystemEvents(EntityId entId, ParticlesUpdateFrameInfo& updateInfo) {
    EParticlesEmitEventType flags = EParticlesEmitEventType::None;
    if(updateInfo.systemStarted) {
        flags = EnumFlagsUnite(flags, EParticlesEmitEventType::OnSystemStart);
    }
    if(updateInfo.systemStopped) {
        flags = EnumFlagsUnite(flags, EParticlesEmitEventType::OnSystemStop);
    }
    if(updateInfo.particlesSpawned) {
        flags = EnumFlagsUnite(flags, EParticlesEmitEventType::OnParticleSpawn);
    }
    if(updateInfo.particlesRemoved) {
        flags = EnumFlagsUnite(flags, EParticlesEmitEventType::OnParticleDeath);
    }
    if(flags == EParticlesEmitEventType::None) {
        return;
    }
    ET_SendEvent(entId, &ETParticlesSystemEvent::ET_onEmitEvent, flags);
}