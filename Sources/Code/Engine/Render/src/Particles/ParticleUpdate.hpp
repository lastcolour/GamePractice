#ifndef __PARTICLE_UPDATE_HPP__
#define __PARTICLE_UPDATE_HPP__

#include "Particles/Particle.hpp"
#include "Render/ParticlesEmitterConfig.hpp"
#include "Math/Transform.hpp"
#include "Math/Random.hpp"

#include <vector>

enum class EmissionState {
    StartDelay = 0,
    Alive,
    WaitingAlive,
    Finished
};

struct EmitterParticles {
public:

    EmitterParticles();

    void start();
    void stop();
    void stopEmitting();
    bool isEmitting() const;
    bool isFinished() const;
    bool hasAlive() const;
    void update(const Transform& tm, float dt);

private:

    void removeOld(float dt);
    void emitNew(const Transform& tm, float dt);
    void updateAlive(float dt);
    void updateState(float dt);
    void spawnNewParticle(const Transform& tm, Particle& p);
    void simulateGravities(float dt);

public:

    ParticleEmitterEmissionConfig emissionConfig;
    ParticleEmitterColorConfig colorConfig;
    ParticleEmitterMovementConfig movementConfig;
    ParticleEmitterGravityFields gravityConfig;

    std::vector<Particle> particles;
    std::vector<ParticleInstanceData> instaceData;

    float duration;
    float emitFracTime;
    int activeCount;
    EmissionState emissionState;

    Math::RandomFloatGenerator floatGen;
};

#endif /* __PARTICLE_UPDATE_HPP__ */