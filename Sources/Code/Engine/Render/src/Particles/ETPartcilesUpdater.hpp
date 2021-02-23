#ifndef __ET_PARTICLES_UPDATER_HPP__
#define __ET_PARTICLES_UPDATER_HPP__

class ParticleEmitterColorConfig;
class ParticleEmitterMovementConfig;
class ParticleEmitterEmissionConfig;

struct ETPartcilesUpdater {
    virtual ~ETPartcilesUpdater() = 0;
    virtual void ET_setColorConfig(const ParticleEmitterColorConfig& newColorConf) = 0;
    virtual void ET_setMovementConfig(const ParticleEmitterMovementConfig& newMovementConf) = 0;
    virtual void ET_setEmissionConfig(const ParticleEmitterEmissionConfig& newEmissionConf) = 0;
};

#endif /* __ET_PARTICLES_UPDATER_HPP__ */