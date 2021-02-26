#ifndef __PARTICLES_EMITTERS_POOL_HPP__
#define __PARTICLES_EMITTERS_POOL_HPP__

#include "Math/Transform.hpp"
#include "Render/ParticlesEmitterConfig.hpp"
#include "Particles/EmitterParticles.hpp"

#include <atomic>
#include <vector>

const int InvalidRootParticleId = -1;

class ParticlesEmittersPool {
public:

    ParticlesEmittersPool();
    ~ParticlesEmittersPool();

    void createEmitter(const EmitRequest& emitReq);
    void updateEmitterPos(int rootParticleId, const Vec2& newPt);
    void stopEmitter(int rootParticleId);
    void simulate(const SimulationConfig& simConfig, const Transform& systemTm, float dt);
    bool hasAlive() const;

    void asyncStopEmitting();
    void asyncDestroyAll();
    bool asyncHasAlive() const;

    const std::vector<std::unique_ptr<EmitterParticles>>& getEmitters() const;

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
};

#endif /* __PARTICLES_EMITTERS_POOL_HPP__ */