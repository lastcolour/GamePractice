#ifndef __ET_PARTICLES_SYSTEM_HPP__
#define __ET_PARTICLES_SYSTEM_HPP__

#include "Render/ParticlesEmitterConfig.hpp"

enum class EParticlesEmitEventType {
    None = 0,
    OnSystemStart = 1 << 0,
    OnParticleSpawn = 1 << 2,
    OnParticleDeath = 1 << 3,
    OnSystemStop = 1 << 4
};

struct ETParticlesSystem {
    virtual ~ETParticlesSystem() = default;
    virtual void ET_setColorConfig(const ParticlesEmitterColorConfig& newColorConf) = 0;
    virtual const ParticlesEmitterColorConfig& ET_getColorConfig() const = 0;
    virtual void ET_setMovementConfig(const ParticlesEmitterMovementConfig& newMovementConf) = 0;
    virtual const ParticlesEmitterMovementConfig& ET_getMovementConfig() const = 0;
    virtual void ET_setEmissionConfig(const ParticlesEmitterEmissionConfig& newEmissionConf) = 0;
    virtual const ParticlesEmitterEmissionConfig& ET_getEmissionConfig() const = 0;
    virtual void ET_setRenderConfig(const ParticlesEmitterRenderConfig& newRenerConf) = 0;
    virtual const ParticlesEmitterRenderConfig& ET_getRenderConfig() const = 0;
    virtual void ET_setSubEmittersConfig(const ParticlesSubEmittersConfig& newSubEmittersConf) = 0;
    virtual const ParticlesSubEmittersConfig& ET_getSubEmittersConfig() const = 0;
    virtual void ET_setSizeConfig(const ParticlesEmitterSizeConfig& newSizeConf) = 0;
    virtual const ParticlesEmitterSizeConfig& ET_getSizeConfig() const = 0;

    virtual void ET_emit() = 0;
    virtual void ET_emitWithTm(const Transform& emitTm) = 0;
    virtual void ET_emitTrackingEntity(EntityId trackEntId) = 0;
    virtual void ET_stopTrackedEmitter(EntityId trackEntId) = 0;
    virtual void ET_stopEmitting() = 0;
    virtual void ET_removeAll() = 0;
    virtual void ET_spawnSubEmitter(int rootParticleId, const Transform& spawnTm) = 0;
    virtual void ET_updateSubEmitter(int rootParticleId, const Transform& newTm) = 0;
    virtual void ET_stopSubEmitter(int rootParticleId) = 0;
    virtual bool ET_hasAliveParticles() const = 0;
};

struct ETParticlesSystemEvent {
    virtual ~ETParticlesSystemEvent() = default;
    virtual void ET_onEmitEvent(EParticlesEmitEventType eventType) = 0;
};

#endif /* __ET_PARTICLES_SYSTEM_HPP__ */