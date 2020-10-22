#ifndef __PARTICLE_UPDATE_HPP__
#define __PARTICLE_UPDATE_HPP__

#include "Particles/Particle.hpp"
#include "Particles/ParticlesEmitterConfig.hpp"
#include "Math/Transform.hpp"

#include <vector>
#include <random>

enum class EmitterLifeType {
    StartDelay = 0,
    Alive,
    WaitingAlive,
    Finished
};

struct EmitterState {
public:

    EmitterState();

    void reset();
    void update(const Transform& tm, float dt);

private:

    void removeOld(float dt);
    void emitNew(const Transform& tm, float dt);
    void updateAlive(float dt);
    void updateState(float dt);
    void spawnNewParticle(const Transform& tm, Particle& p);

    float getRandomFloatInRange(float start, float end);

public:

    ParticleEmitterEmissionConfig emissionConfig;
    ParticleEmitterColorConfig colorConfig;
    ParticleEmitterMovementConfig movementConifg;

    std::vector<Particle> particles;
    std::vector<ParticleInstanceData> instaceData;

    float duration;
    float emitFracTime;
    int activeCount;
    EmitterLifeType lifeType;

    std::default_random_engine randomGenerator;
    std::uniform_real_distribution<float> randomDistribution;
};

#endif /* __PARTICLE_UPDATE_HPP__ */