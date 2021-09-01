#ifndef __PARTICLES_EMITTER_HPP__
#define __PARTICLES_EMITTER_HPP__

#include "Logics/RenderNode.hpp"
#include "Render/ETParticlesSystem.hpp"
#include "Render/ParticlesEmitterConfig.hpp"
#include "Particles/EmitterParticles.hpp"

class ParticlesSystem : public RenderNode,
    public ETNode<ETParticlesSystem>,
    public ETNode<ETParticlesUpdate> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ParticlesSystem();
    virtual ~ParticlesSystem();

    // ETParticlesSystem
    void ET_setColorConfig(const ParticlesEmitterColorConfig& newColorConf) override;
    const ParticlesEmitterColorConfig& ET_getColorConfig() const override;
    void ET_setMovementConfig(const ParticlesEmitterMovementConfig& newMovementConf) override;
    const ParticlesEmitterMovementConfig& ET_getMovementConfig() const override;
    void ET_setEmissionConfig(const ParticlesEmitterEmissionConfig& newEmissionConf) override;
    const ParticlesEmitterEmissionConfig& ET_getEmissionConfig() const override;
    void ET_setRenderConfig(const ParticlesEmitterRenderConfig& newRenerConf) override;
    const ParticlesEmitterRenderConfig& ET_getRenderConfig() const override;
    void ET_setSubEmittersConfig(const ParticlesSubEmittersConfig& newSubEmittersConf) override;
    const ParticlesSubEmittersConfig& ET_getSubEmittersConfig() const override;
    void ET_setSizeConfig(const ParticlesEmitterSizeConfig& newSizeConf) override;
    const ParticlesEmitterSizeConfig& ET_getSizeConfig() const override;

    void ET_emit() override;
    void ET_emitWithTm(const Transform& emitTm) override;
    void ET_emitTrackingEntity(EntityId trackEntId) override;
    void ET_stopTrackedEmitter(EntityId trackEntId) override;
    void ET_stopEmitting() override;
    void ET_removeAll() override;
    void ET_spawnSubEmitter(int rootParticleId, const Transform& spawnTm) override;
    void ET_updateSubEmitter(int rootParticleId, const Transform& newTm) override;
    void ET_stopSubEmitter(int rootParticleId) override;
    bool ET_hasAliveParticles() const override;

    // ETParticlesUpdate
    void ET_updateEmitter(float dt) override;

protected:

    // RenderNode
    void onInit() override;

private:

    ParticlesEmitterEmissionConfig emissionConfig;
    ParticlesEmitterMovementConfig movementConfig;
    ParticlesEmitterColorConfig colorConfig;
    ParticlesEmitterRenderConfig renderConfig;
    ParticlesEmitterGravityConfig gravityConfig;
    ParticlesSubEmittersConfig subEmittersConfig;
    ParticlesEmitterSizeConfig sizeConfig;
};

#endif /* __PARTICLES_EMITTER_HPP__ */