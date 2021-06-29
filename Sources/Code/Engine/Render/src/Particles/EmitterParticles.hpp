#ifndef __EMITTER_PARTICLES_HPP__
#define __EMITTER_PARTICLES_HPP__

#include "Particles/Particle.hpp"
#include "Render/ParticlesEmitterConfig.hpp"
#include "Math/Random.hpp"

#include <atomic>

class ParticlesEmittersPool;

struct SimulationConfig {
    ParticlesEmitterEmissionConfig emission;
    ParticlesEmitterColorConfig color;
    ParticlesEmitterSizeConfig sizeConfig;
    ParticlesEmitterMovementConfig movement;
    ParticlesEmitterGravityConfig gravity;
    ParticlesSubEmittersConfig subEmittorsConfig;
};

enum class EParticleTMTrackType {
    None = 0,
    System,
    Entity
};

struct EmitRequest {
    Transform tm;
    EntityId trackEntId;
    int rootParticleId;
    EParticleTMTrackType tmTrackType;

public:

    EmitRequest() :
        rootParticleId(-1),
        tmTrackType(EParticleTMTrackType::None) {}
};

enum class EmissionState {
    StartDelay = 0,
    Alive,
    WaitingAlive,
    Finished
};

struct EmitterParticles {
public:

    EmitterParticles(ParticlesEmittersPool& pool);
    ~EmitterParticles();

    void start(const EmitRequest& emitRequest);
    void stop();
    void stopEmitting();
    bool isEmitting() const;
    bool isFinished() const;
    bool hasAlive() const;
    void simulate(const Transform& systemTm, float dt);
    Mat4 getTransformMat() const;
    int getRootParticleId() const;
    void updateSubEmitterTm(const Transform& newTm);

private:

    void removeOld(float dt);
    void emitNew(float dt);
    void moveAlive(float dt);
    void updateState(float dt);
    void updateTransform(const Transform& systemTm);
    void spawnNewParticle(Particle& p);
    void simulateGravities(const Transform& tm, float dt);
    void updateIntacesData(float dt);
    int generateParticleId();

public:

    std::vector<Particle> particles;
    std::vector<ParticleInstanceData> instaceData;
    int activeCount;

private:

    EmitRequest emitReq;
    ParticlesEmittersPool& pool;
    const SimulationConfig& simConf;
    float duration;
    float emitFracTime;
    Math::RandomFloatGenerator floatGen;
    unsigned int spawnedCount;
    std::atomic<EmissionState> emissionState;
    SubEmitterTriggerEvent subEmitterFlags;
};

#endif /* __EMITTER_PARTICLES_HPP__ */