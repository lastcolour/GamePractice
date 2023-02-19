#ifndef __PARTICLES_EMITTERS_POOL_HPP__
#define __PARTICLES_EMITTERS_POOL_HPP__

#include "Render/ParticlesEmitterConfig.hpp"
#include "Particles/EmitterParticles.hpp"
#include "Particles/ParticlesUpdateFrameInfo.hpp"

#include <atomic>

const int InvalidRootParticleId = -1;

class ParticlesEmittersPool {
public:

    ParticlesEmittersPool();
    ~ParticlesEmittersPool();

    void createEmitter(const EmitRequest& emitReq);
    void stopTrackEmitter(EntityId trackEntId);
    void updateSubEmitterTm(int rootParticleId, const Transform& newTm);
    void stopEmitter(int rootParticleId);
    void simulate(const Transform& systemTm, float dt);
    bool hasAlive() const;
    SimulationConfig& getSimConfig();
    const SimulationConfig& getSimConfig() const;

    ParticlesUpdateFrameInfo& getUpdateInfo();
    ParticlesUpdateFrameInfo getAndResetUpdateInfo();

    void stopEmittingAll();
    void destroyAll();
    bool isStopped() const;

    const std::vector<std::unique_ptr<EmitterParticles>>& getEmitters() const;

    int addParticles(int count);
    void removeParticles(int count);
    int getParticlesCount() const;

private:

    enum class EmittingState {
        Playing = 0,
        Stopping,
        Stopped
    };

private:

    ParticlesUpdateFrameInfo updateFrameInfo;
    EmittingState emittingState;
    std::vector<std::unique_ptr<EmitterParticles>> pool;
    SimulationConfig simConfig;
    int particlesCount;
};

#endif /* __PARTICLES_EMITTERS_POOL_HPP__ */