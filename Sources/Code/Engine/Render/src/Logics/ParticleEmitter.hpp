#ifndef __PARTICLE_EMITTER_HPP__
#define __PARTICLE_EMITTER_HPP__

#include "Logics/RenderNode.hpp"
#include "Render/ETPatricle.hpp"
#include "Render/ParticlesEmitterConfig.hpp"
#include "Nodes/ETRenderNodeManager.hpp"

#include <atomic>

class ParticleEmitter : RenderNode,
    public ETNode<ETParticleEmitter>,
    public ETNode<ETParticleUpdate> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    enum class State {
        PlayRequested = 0,
        Playing,
        StopRequested,
        Stopping,
        Stopped
    };

public:

    ParticleEmitter();
    virtual ~ParticleEmitter();

    // EntityLogic
    void init() override;

    // ETParticleEmitter
    void ET_setColorConfig(const ParticleEmitterColorConfig& newColorConf) override;
    const ParticleEmitterColorConfig& ET_getColorConfig() const override;
    void ET_setMovementConfig(const ParticleEmitterMovementConfig& newMovementConf) override;
    const ParticleEmitterMovementConfig& ET_getMovementConfig() const override;
    void ET_setEmissionConfig(const ParticleEmitterEmissionConfig& newEmissionConf) override;
    const ParticleEmitterEmissionConfig& ET_getEmissionConfig() const override;
    void ET_setRenderConfig(const ParticleEmitterRenderConfig& newRenerConf) override;
    const ParticleEmitterRenderConfig& ET_getRenderConfig() const override;
    void ET_emit() override;
    bool ET_isPlaying() const override;

    // ETParticleUpdate
    void ET_updateEmitter(float dt) override;

protected:

    void onSyncWithRender() override;

private:

    ParticleEmitterEmissionConfig emissionConfig;
    ParticleEmitterMovementConfig movementConfig;
    ParticleEmitterColorConfig colorConfig;
    ParticleEmitterRenderConfig renderConfig;
    ParticleEmitterGravityFields gravityConfig;
    std::atomic<State> state;
    bool canUpdate;
    bool isEmissionConfigChanged;
    bool isMovementConfigChanged;
    bool isColorConfigChanged;
    bool isRenderConfigChanged;
    bool isGravityConfigChanged;
};

#endif /* __PARTICLE_EMITTER_HPP__ */