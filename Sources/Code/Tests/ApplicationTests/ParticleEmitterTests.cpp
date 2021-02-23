#include "ParticleEmitterTests.hpp"
#include "Logics/ParticleEmitter.hpp"
#include "Render/ParticlesEmitterConfig.hpp"
#include "Nodes/ETRenderNodeManager.hpp"

namespace {

void TickParticleEmitter(float dt) {
    ET_SendEvent(&ETRenderProxyNodeEvents::ET_syncWithRender);
    ET_SendEvent(&ETParticleUpdate::ET_updateEmitter, 10.f);
    ET_SendEvent(&ETParticleUpdate::ET_updateEmitter, 0.f);
}

} // namespace

TEST_F(ParticleEmitterTests, CheckParticleEmitterPlaying) {
    auto entity = createVoidObject();
    auto emitter = entity->addCustomLogic<ParticleEmitter>();

    auto emissionConfig = emitter->ET_getEmissionConfig();
    emissionConfig.lifetime = 1.f;
    emissionConfig.duration = 1.f;
    emissionConfig.emissionRate = 64.f;
    emissionConfig.autoStart = false;
    emissionConfig.loop = false;

    emitter->ET_setEmissionConfig(emissionConfig);

    emitter->ET_emit();

    EXPECT_TRUE(emitter->ET_isPlaying());

    TickParticleEmitter(10.f);

    EXPECT_FALSE(emitter->ET_isPlaying());
}