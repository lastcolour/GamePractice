#include "ParticlesSystemTests.hpp"
#include "Logics/ParticlesSystem.hpp"
#include "Render/ParticlesEmitterConfig.hpp"
#include "Nodes/ETRenderNodeManager.hpp"
#include "Particles/ParticlesEmittersPool.hpp"
#include "Nodes/ParticlesNode.hpp"
#include "Core/GlobalData.hpp"
#include "RenderConfig.hpp"

namespace {

void TickParticlesEmitter(float dt) {
    ET_PollAllEvents<ETRenderNodeManager>();
    ET_SendEvent(&ETParticlesUpdate::ET_updateEmitter, dt);
    ET_SendEvent(&ETParticlesUpdate::ET_updateEmitter, 0.f);
}

class TestParticlesSystemLogic : public ParticlesSystem {
public:

    TestParticlesSystemLogic() = default;
    virtual ~TestParticlesSystemLogic() = default;

    ParticlesEmittersPool* getEmittersPool() {
        if(!proxyNode) {
            return nullptr;
        }
        auto particlesNode = static_cast<ParticlesNode*>(proxyNode);
        return &particlesNode->getEmittersPool();
    } 
};

void checkParticlePosition(const Vec2& expectedPt, int id, const EmitterParticles& particles) {
    auto& p = particles.instaceData[id];

    auto tmMat = p.mat.toMat3();
    Vec3 pt1 = tmMat * Vec3(0.f, 0.f, 1.f);

    auto modelMat = particles.getTransformMat();
    Vec4 resPt = modelMat * Vec4(pt1.x, pt1.y, 0.f, 1.f);

    EXPECT_FLOAT_EQ(resPt.x, expectedPt.x);
    EXPECT_FLOAT_EQ(resPt.y, expectedPt.y);
}

} // namespace

TEST_F(ParticlesSystemTests, CheckParticlesEmitterPlaying) {
    auto entity = createVoidObject();
    auto system = entity->addCustomLogic<TestParticlesSystemLogic>();

    auto emissionConfig = system->ET_getEmissionConfig();
    emissionConfig.lifetime = 5.f;
    emissionConfig.duration = 1.f;
    emissionConfig.emissionRate = 64.f;
    emissionConfig.autoStart = false;
    emissionConfig.loop = false;

    system->ET_setEmissionConfig(emissionConfig);
    system->ET_removeAll();
    system->ET_emit();

    TickParticlesEmitter(1.f);

    auto pool = system->getEmittersPool();
    ASSERT_TRUE(pool);

    {
        auto& emitters = pool->getEmitters();
        ASSERT_EQ(emitters.size(), 1);

        auto& particles = emitters[0];

        EXPECT_FALSE(particles->isFinished());
        EXPECT_TRUE(particles->hasAlive());
        EXPECT_EQ(particles->getRootParticleId(), -1);

        EXPECT_EQ(particles->activeCount, 64);

        EXPECT_TRUE(system->ET_hasAliveParticles());
    }

    TickParticlesEmitter(10.f);

    {
        auto& emitters = pool->getEmitters();
        EXPECT_EQ(emitters.size(), 1);

        auto& particles = emitters[0];
    
        EXPECT_TRUE(particles->isFinished());
        EXPECT_FALSE(particles->hasAlive());
        EXPECT_EQ(particles->getRootParticleId(), -1);

        EXPECT_EQ(particles->activeCount, 0);

        EXPECT_FALSE(system->ET_hasAliveParticles());
    }
}

TEST_F(ParticlesSystemTests, CheckWorldTM) {
    Vec2 emitPt(100, 200);

    auto entity = createVoidObject();
    {
        Transform tm;
        tm.pt = Vec3(emitPt.x, emitPt.y, 0.f);
        entity->ET_setTransform(tm);
    }
    auto system = entity->addCustomLogic<TestParticlesSystemLogic>();

    auto emissionConfig = system->ET_getEmissionConfig();
    emissionConfig.lifetime = 5.f;
    emissionConfig.duration = 1.f;
    emissionConfig.emissionRate = 1.f;
    emissionConfig.autoStart = false;
    emissionConfig.loop = false;
    emissionConfig.emitterVal = Vec2(0.f);
    emissionConfig.emitterSpace = EmitterSpace::World;

    auto movementConfig = system->ET_getMovementConfig();
    movementConfig.speed = 0.f;
    movementConfig.speedVar = 0.f;

    system->ET_setEmissionConfig(emissionConfig);
    system->ET_setMovementConfig(movementConfig);
    system->ET_removeAll();
    system->ET_emit();

    TickParticlesEmitter(1.f);

    auto pool = system->getEmittersPool();
    ASSERT_TRUE(pool);

    {
        auto& emitters = pool->getEmitters();
        ASSERT_EQ(emitters.size(), 1);

        auto& particles = *emitters[0];
        ASSERT_EQ(particles.activeCount, 1);

        checkParticlePosition(emitPt, 0, particles);
    }

    Vec2 newEmitPt(300, 400);

    {
        Transform tm;
        tm.pt = Vec3(emitPt.x, emitPt.y, 0.f);
        entity->ET_setTransform(tm);
    }

    TickParticlesEmitter(1.f);

    {
        auto& emitters = pool->getEmitters();
        ASSERT_EQ(emitters.size(), 1);

        auto& particles = *emitters[0];
        ASSERT_EQ(particles.activeCount, 1);

        checkParticlePosition(emitPt, 0, particles);
    }
}

TEST_F(ParticlesSystemTests, CheckLocalTm) {
    Vec2 emitPt(100, 200);

    auto entity = createVoidObject();
    {
        Transform tm;
        tm.pt = Vec3(emitPt.x, emitPt.y, 0.f);
        entity->ET_setTransform(tm);
    }
    auto system = entity->addCustomLogic<TestParticlesSystemLogic>();

    auto emissionConfig = system->ET_getEmissionConfig();
    emissionConfig.lifetime = 5.f;
    emissionConfig.duration = 1.f;
    emissionConfig.emissionRate = 1.f;
    emissionConfig.autoStart = false;
    emissionConfig.loop = false;
    emissionConfig.emitterVal = Vec2(0.f);
    emissionConfig.emitterSpace = EmitterSpace::Local;

    auto movementConfig = system->ET_getMovementConfig();
    movementConfig.speed = 0.f;
    movementConfig.speedVar = 0.f;

    system->ET_setEmissionConfig(emissionConfig);
    system->ET_setMovementConfig(movementConfig);
    system->ET_removeAll();
    system->ET_emit();

    TickParticlesEmitter(1.f);

    auto pool = system->getEmittersPool();
    ASSERT_TRUE(pool);

    {
        auto& emitters = pool->getEmitters();
        ASSERT_EQ(emitters.size(), 1);

        auto& particles = *emitters[0];
        ASSERT_EQ(particles.activeCount, 1);

        checkParticlePosition(emitPt, 0, particles);
    }

    Vec2 newEmitPt(300, 400);

    {
        Transform tm;
        tm.pt = Vec3(newEmitPt.x, newEmitPt.y, 0.f);
        entity->ET_setTransform(tm);
    }

    TickParticlesEmitter(1.f);

    {
        auto& emitters = pool->getEmitters();
        ASSERT_EQ(emitters.size(), 1);

        auto& particles = *emitters[0];
        ASSERT_EQ(particles.activeCount, 1);

        checkParticlePosition(newEmitPt, 0, particles);
    }
}

TEST_F(ParticlesSystemTests, CheckSubEmitters) {
    EntityId parentId;
    EntityId childId;
    {
        auto parent = createVoidObject();
        parentId = parent->getEntityId();

        auto system = parent->addCustomLogic<TestParticlesSystemLogic>();

        auto emissionConfig = system->ET_getEmissionConfig();
        emissionConfig.lifetime = 1.f;
        emissionConfig.duration = 1.f;
        emissionConfig.emissionRate = 64.f;
        emissionConfig.autoStart = false;
        emissionConfig.loop = false;

        system->ET_setEmissionConfig(emissionConfig);
    }
    {
        auto child = createVoidObject();
        childId = child->getEntityId();

        auto system = child->addCustomLogic<TestParticlesSystemLogic>();

        auto emissionConfig = system->ET_getEmissionConfig();
        emissionConfig.lifetime = 1.f;
        emissionConfig.duration = 1.f;
        emissionConfig.emissionRate = 64.f;
        emissionConfig.autoStart = false;
        emissionConfig.loop = false;

        system->ET_setEmissionConfig(emissionConfig);
    }

    {
        ParticlesSubEmittersConfig subEmittersConfig;
        subEmittersConfig.subEmitters.emplace_back(SubEmitterTriggerEvent::OnStart, childId);
        subEmittersConfig.subEmitters.emplace_back(SubEmitterTriggerEvent::OnEnd, childId);
        subEmittersConfig.subEmitters.emplace_back(SubEmitterTriggerEvent::OnParticleSpawn, childId);
        subEmittersConfig.subEmitters.emplace_back(SubEmitterTriggerEvent::OnParticleDeath, childId);
        subEmittersConfig.subEmitters.emplace_back(SubEmitterTriggerEvent::OnParticleLive, childId);

        ET_SendEvent(parentId, &ETParticlesSystem::ET_setSubEmittersConfig, subEmittersConfig);
    }

    ET_SendEvent(parentId, &ETParticlesSystem::ET_emit);
    TickParticlesEmitter(0.f);
}

TEST_F(ParticlesSystemTests, CheckMaxParticles) {
    auto entity = createVoidObject();
    auto system = entity->addCustomLogic<TestParticlesSystemLogic>();

    auto maxParticles = GetGlobal<RenderConfig>()->particlesConfig.maxParticles;

    auto emissionConfig = system->ET_getEmissionConfig();
    emissionConfig.lifetime = 5.f;
    emissionConfig.duration = 1.f;
    emissionConfig.emissionRate = maxParticles * 2.f;
    emissionConfig.autoStart = false;
    emissionConfig.loop = false;

    system->ET_setEmissionConfig(emissionConfig);
    system->ET_emit();

    TickParticlesEmitter(64.f);

    auto pool = system->getEmittersPool();
    ASSERT_TRUE(pool);

    {
        auto& emitters = pool->getEmitters();
        ASSERT_EQ(emitters.size(), 1);

        auto& particles = emitters[0];
        EXPECT_EQ(particles->activeCount, maxParticles);
    }
}