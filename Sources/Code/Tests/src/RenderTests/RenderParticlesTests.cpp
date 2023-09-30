#include "RenderTests/RenderParticlesTests.hpp"
#include "Logics/ParticlesSystem.hpp"
#include "Render/src/Commands/Commands.hpp"
#include "Render/src/RenderConfig.hpp"

namespace {

const char* TEST_IMAGE = "Images/Base/block_1x1.png";

void makeVisisble(ParticlesSystem& system) {
    auto renderConfig = system.ET_getRenderConfig();
    renderConfig.textureInfo.filename = TEST_IMAGE;
    system.ET_setRenderConfig(renderConfig);
}

class TestParticlesSystemLogic : public ParticlesSystem {
public:

    TestParticlesSystemLogic() = default;
    virtual ~TestParticlesSystemLogic() = default;

    ParticlesEmittersPool* getEmittersPool() {
        if(!cmd) {
            return nullptr;
        }
        auto particlesCmd = static_cast<DrawParticlesCmd*>(cmd);
        return &particlesCmd->emittersPool;
    } 
};

void checkParticlePosition(const Vec2& expectedPt, int id, const EmitterParticles& particles) {
    auto& p = particles.instanceData[id];

    auto tmMat = p.mat.toMat3();
    Vec3 pt1 = tmMat * Vec3(0.f, 0.f, 1.f);

    auto modelMat = particles.getTransformMat();
    Vec4 resPt = modelMat * Vec4(pt1.x, pt1.y, 0.f, 1.f);

    EXPECT_FLOAT_EQ(resPt.x, expectedPt.x);
    EXPECT_FLOAT_EQ(resPt.y, expectedPt.y);
}

class TestParticlesSystemEventListener : public EntityLogic,
    public ETNode<ETParticlesSystemEvent> {
public:

    TestParticlesSystemEventListener() {}
    virtual ~TestParticlesSystemEventListener() {}

    // EntityLogic
    void init() override {
        ETNode<ETParticlesSystemEvent>::connect(getEntityId());
    }

    void deinit() override {
    }

    void ET_onEmitEvent(EParticlesEmitEventType eventType) override {
        events.push_back(eventType);
    }

public:

    std::vector<EParticlesEmitEventType> events;
};

} // namespace

void RenderParticlesTests::TickParticleSystems(float dt) {
    syncWithGameAndDrawToImageBuffer(dt);
    syncWithGameAndDrawToImageBuffer(0.f);
}

TEST_F(RenderParticlesTests, CheckParticlesEmitterPlaying) {
    auto entity = createVoidObject();
    auto system = entity->addCustomLogic<TestParticlesSystemLogic>();
    makeVisisble(*system);

    auto emissionConfig = system->ET_getEmissionConfig();
    emissionConfig.lifetime = 5.f;
    emissionConfig.duration = 1.f;
    emissionConfig.emissionRate = 64.f;
    emissionConfig.autoStart = false;
    emissionConfig.loop = false;

    system->ET_setEmissionConfig(emissionConfig);
    system->ET_removeAll();
    system->ET_emit();

    {
        float dt = 1.f;
        syncWithGameAndDrawToImageBuffer(dt);
        syncWithGameAndDrawToImageBuffer(0.f);
    }

    auto pool = system->getEmittersPool();
    ASSERT_TRUE(pool);

    {
        auto& emitters = pool->getEmitters();
        ASSERT_EQ(emitters.size(), 1u);

        auto& particles = emitters[0];

        EXPECT_FALSE(particles->isFinished());
        EXPECT_TRUE(particles->hasAlive());
        EXPECT_EQ(particles->getEmitRequest().rootParticleId, -1);

        EXPECT_EQ(particles->activeCount, 64);

        EXPECT_TRUE(system->ET_hasAliveParticles());
    }

    TickParticleSystems(10.f);

    {
        auto& emitters = pool->getEmitters();
        EXPECT_EQ(emitters.size(), 1);

        auto& particles = emitters[0];
    
        EXPECT_TRUE(particles->isFinished());
        EXPECT_FALSE(particles->hasAlive());
        EXPECT_EQ(particles->getEmitRequest().rootParticleId, -1);

        EXPECT_EQ(particles->activeCount, 0);

        EXPECT_FALSE(system->ET_hasAliveParticles());
    }
}

TEST_F(RenderParticlesTests, CheckWorldTM) {
    Vec2 emitPt(100, 200);

    auto entity = createVoidObject();
    {
        Transform tm;
        tm.pt = Vec3(emitPt.x, emitPt.y, 0.f);
        entity->ET_setTransform(tm);
    }
    auto system = entity->addCustomLogic<TestParticlesSystemLogic>();
    makeVisisble(*system);

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

    TickParticleSystems(1.f);

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

    TickParticleSystems(1.f);

    {
        auto& emitters = pool->getEmitters();
        ASSERT_EQ(emitters.size(), 1);

        auto& particles = *emitters[0];
        ASSERT_EQ(particles.activeCount, 1);

        checkParticlePosition(emitPt, 0, particles);
    }
}

TEST_F(RenderParticlesTests, CheckLocalTm) {
    Vec2 emitPt(100, 200);

    auto entity = createVoidObject();
    {
        Transform tm;
        tm.pt = Vec3(emitPt.x, emitPt.y, 0.f);
        entity->ET_setTransform(tm);
    }
    auto system = entity->addCustomLogic<TestParticlesSystemLogic>();
    makeVisisble(*system);

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

    TickParticleSystems(1.f);

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

    TickParticleSystems(1.f);

    {
        auto& emitters = pool->getEmitters();
        ASSERT_EQ(emitters.size(), 1);

        auto& particles = *emitters[0];
        ASSERT_EQ(particles.activeCount, 1);

        checkParticlePosition(newEmitPt, 0, particles);
    }
}

TEST_F(RenderParticlesTests, CheckSubEmitters) {
    EntityId parentId;
    EntityId childId;
    {
        auto parent = createVoidObject();
        parentId = parent->getEntityId();

        auto system = parent->addCustomLogic<TestParticlesSystemLogic>();
        makeVisisble(*system);

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
        makeVisisble(*system);

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

    TickParticleSystems(1.f);
}

TEST_F(RenderParticlesTests, CheckMaxParticles) {
    auto entity = createVoidObject();
    auto system = entity->addCustomLogic<TestParticlesSystemLogic>();
    makeVisisble(*system);

    auto maxParticles = Core::GetGlobal<RenderConfig>()->particlesConfig.maxParticles;

    auto emissionConfig = system->ET_getEmissionConfig();
    emissionConfig.lifetime = 5.f;
    emissionConfig.duration = 1.f;
    emissionConfig.emissionRate = maxParticles * 2.f;
    emissionConfig.autoStart = false;
    emissionConfig.loop = false;

    system->ET_setEmissionConfig(emissionConfig);
    system->ET_emit();

    TickParticleSystems(64.f);

    auto pool = system->getEmittersPool();
    ASSERT_TRUE(pool);

    {
        auto& emitters = pool->getEmitters();
        ASSERT_EQ(emitters.size(), 1);

        auto& particles = emitters[0];
        EXPECT_EQ(particles->activeCount, maxParticles);
    }
}

TEST_F(RenderParticlesTests, CheckParticleSystemEvents) {
    auto entity = createVoidObject();
    auto system = entity->addCustomLogic<TestParticlesSystemLogic>();
    makeVisisble(*system);

    auto eventListener = entity->addCustomLogic<TestParticlesSystemEventListener>();

    {
        auto emissionConfig = system->ET_getEmissionConfig();
        emissionConfig.lifetime = 1.f;
        emissionConfig.duration = 1.f;
        emissionConfig.emissionRate = 2.5f;
        emissionConfig.autoStart = false;
        emissionConfig.loop = false;
        system->ET_setEmissionConfig(emissionConfig);
    }

    EXPECT_EQ(eventListener->events.size(), 0);

    system->ET_emit();

    TickParticleSystems(0.f);

    {
        ASSERT_EQ(eventListener->events.size(), 1);
        ASSERT_EQ(eventListener->events[0], EParticlesEmitEventType::OnSystemStart);
        eventListener->events.clear();
    }

    TickParticleSystems(0.5f);

    {
        ASSERT_EQ(eventListener->events.size(), 1);
        ASSERT_EQ(eventListener->events[0], EParticlesEmitEventType::OnParticleSpawn);
        eventListener->events.clear();
    }

    TickParticleSystems(0.4f);

    {
        ASSERT_EQ(eventListener->events.size(), 1);
        ASSERT_EQ(eventListener->events[0], EParticlesEmitEventType::OnParticleSpawn);
        eventListener->events.clear();
    }

    TickParticleSystems(0.7f);

    {
        ASSERT_EQ(eventListener->events.size(), 1);
        ASSERT_EQ(eventListener->events[0], EParticlesEmitEventType::OnParticleDeath);
        eventListener->events.clear();
    }

    TickParticleSystems(1.f);

    {
        ASSERT_EQ(eventListener->events.size(), 2);
        ASSERT_EQ(eventListener->events[0], EParticlesEmitEventType::OnParticleDeath);
        ASSERT_EQ(eventListener->events[1], EParticlesEmitEventType::OnSystemStop);
        eventListener->events.clear();
    }
}