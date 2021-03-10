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

    EmitterParticles(const SimulationConfig& config);
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
    void updateEmitPos(const Vec2& newEmitPt);

private:

    void removeOld(float dt);
    void emitNew(float dt);
    void moveAlive(float dt);
    void updateState(float dt);
    void spawnNewParticle(Particle& p);
    void simulateGravities(float dt);
    void updateIntacesData(float dt);
    int generateParticleId();

public:

    std::vector<Particle> particles;
    std::vector<ParticleInstanceData> instaceData;
    int activeCount;

private:

    EmitRequest emitReq;
    const SimulationConfig& simConfig;
    float duration;
    float emitFracTime;
    Math::RandomFloatGenerator floatGen;
    unsigned int spawnedCount;
    std::atomic<EmissionState> emissionState;
    bool wroldTmSpace;
};

#endif /* __EMITTER_PARTICLES_HPP__ */