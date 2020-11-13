#ifndef __PARTICLE_EMITTER_HPP__
#define __PARTICLE_EMITTER_HPP__

#include "Logics/RenderNode.hpp"
#include "Render/ETPatricle.hpp"
#include "Particles/ParticlesEmitterConfig.hpp"

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

protected:

    void onSyncWithRender() override;

private:

    ParticleEmitterEmissionConfig emissionConfig;
    ParticleEmitterMovementConfig movementConfig;
    ParticleEmitterColorConfig colorConfig;
    ParticleEmitterRenderConfig renderConfig;
    bool isConfigChanged;
};

#endif /* __PARTICLE_EMITTER_HPP__ */