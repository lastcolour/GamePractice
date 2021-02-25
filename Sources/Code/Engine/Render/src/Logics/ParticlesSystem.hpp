#ifndef __PARTICLES_EMITTER_HPP__
#define __PARTICLES_EMITTER_HPP__

#include "Logics/RenderNode.hpp"
#include "Render/ETParticlesSystem.hpp"
#include "Render/ParticlesEmitterConfig.hpp"
#include "Nodes/ETRenderNodeManager.hpp"
#include "Particles/EmitterParticles.hpp"

class ParticlesSystem : public RenderNode,
    public ETNode<ETParticlesSystem>,
    public ETNode<ETParticlesUpdate> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ParticlesSystem();
    virtual ~ParticlesSystem();

    // EntityLogic
    void init() override;

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
    void ET_emit() override;
    void ET_emitWithTm(const Transform& emitTm) override;
    void ET_stopEmitting() override;
    void ET_destoryAll() override;
    void ET_spawnSubEmitter(int rootParticleId, const Vec2& pt) override;
    void ET_updateSubEmitter(int rootParticleId, const Vec2& pt) override;
    bool ET_hasAliveParticles() const override;

    // ETParticlesUpdate
    void ET_updateEmitter(float dt) override;

protected:

    void onSyncWithRender() override;

private:

    struct EmitRequest {
        Transform tm;
        int rootParticleId;
    };

private:

    ParticlesEmitterEmissionConfig emissionConfig;
    ParticlesEmitterMovementConfig movementConfig;
    ParticlesEmitterColorConfig colorConfig;
    ParticlesEmitterRenderConfig renderConfig;
    ParticlesEmitterGravityConfig gravityConfig;
    ParticlesSubEmittersConfig subEmittersConfig;
    SimulationConfig simConfig;
    std::vector<EmitRequest> pendingEmits;

    bool canUpdate;
    bool isEmissionConfigChanged;
    bool isMovementConfigChanged;
    bool isColorConfigChanged;
    bool isRenderConfigChanged;
    bool isGravityConfigChanged;
    bool isSubEmittersConfigChanged;
};

#endif /* __PARTICLES_EMITTER_HPP__ */