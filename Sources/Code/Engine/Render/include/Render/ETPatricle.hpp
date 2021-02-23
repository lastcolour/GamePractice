#ifndef __ET_PARTICLE_HPP__
#define __ET_PARTICLE_HPP__

class ParticleEmitterColorConfig;
class ParticleEmitterMovementConfig;
class ParticleEmitterEmissionConfig;
class ParticleEmitterRenderConfig;

struct ETParticleEmitter {
    virtual ~ETParticleEmitter() = default;
    virtual void ET_setColorConfig(const ParticleEmitterColorConfig& newColorConf) = 0;
    virtual const ParticleEmitterColorConfig& ET_getColorConfig() const = 0;
    virtual void ET_setMovementConfig(const ParticleEmitterMovementConfig& newMovementConf) = 0;
    virtual const ParticleEmitterMovementConfig& ET_getMovementConfig() const = 0;
    virtual void ET_setEmissionConfig(const ParticleEmitterEmissionConfig& newEmissionConf) = 0;
    virtual const ParticleEmitterEmissionConfig& ET_getEmissionConfig() const = 0;
    virtual void ET_setRenderConfig(const ParticleEmitterRenderConfig& newRenerConf) = 0;
    virtual const ParticleEmitterRenderConfig& ET_getRenderConfig() const = 0;
    virtual void ET_emit() = 0;
    virtual bool ET_isPlaying() const = 0;
};

#endif /* __ET_PARTICLE_HPP__ */