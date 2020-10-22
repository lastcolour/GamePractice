#ifndef __ET_PARTICLE_HPP__
#define __ET_PARTICLE_HPP__

struct ETParticleEmitter {
    virtual ~ETParticleEmitter() = default;
    virtual void ET_emit() = 0;
};

#endif /* __ET_PARTICLE_HPP__ */