#ifndef __PARTICLES_EMITTERS_POOL_HPP__
#define __PARTICLES_EMITTERS_POOL_HPP__

#include "Render/ParticlesEmitterConfig.hpp"
#include "Particles/EmitterParticles.hpp"

#include <atomic>

const int InvalidRootParticleId = -1;

class ParticlesEmittersPool {
public:

    ParticlesEmittersPool();
    ~ParticlesEmittersPool();

    void createEmitter(const EmitRequest& emitReq);
    void updateSubEmitterTm(int rootParticleId, const Transform& newTm);
    void stopEmitter(int rootParticleId);
    void simulate(const Transform& systemTm, float dt);
    bool hasAlive() const;
    SimulationConfig& getSimConfig();
    const SimulationConfig& getSimConfig() const;

    void asyncStopEmitting();
    void asyncDestroyAll();
    bool asyncHasAlive() const;

    const std::vector<std::unique_ptr<EmitterParticles>>& getEmitters() const;

    int addParticles(int count);
    void removeParticles(int count);

private:

    enum AsynState {
        Playing,
        StopRequested,
        ForceStopRequested,
        Stoppig,
        Stopped,
    };

private:

    std::atomic<AsynState> asyncState;
    std::vector<std::unique_ptr<EmitterParticles>> pool;
    SimulationConfig simConfig;
    int particlesCount;
};

#endif /* __PARTICLES_EMITTERS_POOL_HPP__ */