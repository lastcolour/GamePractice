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
    ParticlesEmitterMovementConfig movement;
    ParticlesEmitterGravityConfig gravity;
    ParticlesSubEmittersConfig subEmittorsConfig;
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

    void start(const Transform& emitTm);
    void stop();
    void stopEmitting();
    bool isEmitting() const;
    bool isFinished() const;
    bool hasAlive() const;
    void simulate(const SimulationConfig& simConfig, const Transform& systemTm, float dt);
    Mat4 getTransformMat() const;
    int getRootParticleId() const;
    void setRootParticleId(int newRootParticleId);

private:

    void removeOld(const SimulationConfig& simConfig, float dt);
    void emitNew(const SimulationConfig& simConfig, float dt);
    void updateAlive(const SimulationConfig& simConfig, float dt);
    void updateState(const SimulationConfig& simConfig, float dt);
    void spawnNewParticle(const SimulationConfig& simConfig, Particle& p);
    void simulateGravities(const SimulationConfig& simConfig, float dt);

public:

    Transform tm;
    std::vector<Particle> particles;
    std::vector<ParticleInstanceData> instaceData;
    int activeCount;

private:

    float duration;
    float emitFracTime;
    std::atomic<EmissionState> emissionState;
    Math::RandomFloatGenerator floatGen;
    int rootParticleId;
    bool wroldTmSpace;
};

#endif /* __EMITTER_PARTICLES_HPP__ */