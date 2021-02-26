#ifndef __EMITTER_PARTICLES_HPP__
#define __EMITTER_PARTICLES_HPP__

#include "Particles/Particle.hpp"
#include "Render/ParticlesEmitterConfig.hpp"
#include "Math/Transform.hpp"
#include "Math/Random.hpp"

#include <vector>
#include <atomic>

struct SimulationConfig {
    ParticlesEmitterEmissionConfig emission;
    ParticlesEmitterColorConfig color;
    ParticlesEmitterSizeConfig sizeConfig;
    ParticlesEmitterMovementConfig movement;
    ParticlesEmitterGravityConfig gravity;
    ParticlesSubEmittersConfig subEmittorsConfig;
};

struct EmitRequest {
    Transform tm;
    int rootParticleId;
    bool syncWithSystemTm;
};

enum class EmissionState {
    StartDelay = 0,
    Alive,
    WaitingAlive,
    Finished
};

struct EmitterParticles {
public:

    EmitterParticles();

    void start(const EmitRequest& emitRequest);
    void stop();
    void stopEmitting();
    bool isEmitting() const;
    bool isFinished() const;
    bool hasAlive() const;
    void simulate(const SimulationConfig& simConfig, const Transform& systemTm, float dt);
    Mat4 getTransformMat() const;
    int getRootParticleId() const;
    void updateEmitPos(const Vec2& newEmitPt);

private:

    void removeOld(const SimulationConfig& simConfig, float dt);
    void emitNew(const SimulationConfig& simConfig, float dt);
    void moveAlive(const SimulationConfig& simConfig, float dt);
    void updateState(const SimulationConfig& simConfig, float dt);
    void spawnNewParticle(const SimulationConfig& simConfig, Particle& p);
    void simulateGravities(const SimulationConfig& simConfig, float dt);
    void updateIntacesData(const SimulationConfig& simConfig, float dt);
    int generateParticleId();

public:

    std::vector<Particle> particles;
    std::vector<ParticleInstanceData> instaceData;
    int activeCount;

private:

    EmitRequest emitReq;
    float duration;
    float emitFracTime;
    Math::RandomFloatGenerator floatGen;
    int spawnedCount;
    std::atomic<EmissionState> emissionState;
    bool wroldTmSpace;
};

#endif /* __EMITTER_PARTICLES_HPP__ */