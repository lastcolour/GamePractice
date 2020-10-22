#ifndef __PARTICLE_EMITTER_HPP__
#define __PARTICLE_EMITTER_HPP__

#include "Logics/RenderNode.hpp"
#include "Render/ETPatricle.hpp"

class ReflectContext;

enum class EmitterType {
    Sphere = 0,
    Cone,
    Box
};

struct Particle {

};

class ParticleEmitter : RenderNode,
    public ETNode<ETParticleEmitter> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ParticleEmitter();
    virtual ~ParticleEmitter();

    // EntityLogic
    bool init() override;

    // ETParticleEmitter
    void ET_emit() override;

private:

    EmitterType emitterType;
    float duration;
    float particleEmissionRate;
    float particleLifetime;
    float particleLifeTimeVar;
    float particleSpeed;
    float particleSpeedVar;
};

#endif /* __PARTICLE_EMITTER_HPP__ */