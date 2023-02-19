#include "Particles/ParticlesEmittersPool.hpp"
#include "RenderConfig.hpp"

#include <cassert>

ParticlesEmittersPool::ParticlesEmittersPool() :
    emittingState(EmittingState::Stopped),
    particlesCount(0) {
}

ParticlesEmittersPool::~ParticlesEmittersPool() {
    pool.clear();
}

const std::vector<std::unique_ptr<EmitterParticles>>& ParticlesEmittersPool::getEmitters() const {
    return pool;
}

void ParticlesEmittersPool::createEmitter(const EmitRequest& emitReq) {
    EmitterParticles* stoppedEmitter = nullptr;
    for(auto& emitter : pool) {
        if(emitter->isFinished()) {
            stoppedEmitter = emitter.get();
        }
    }
    if(!stoppedEmitter) {
        pool.emplace_back(new EmitterParticles(*this));
        stoppedEmitter = pool.back().get();
    }
    emittingState = EmittingState::Playing;
    stoppedEmitter->start(emitReq);
}

void ParticlesEmittersPool::stopTrackEmitter(EntityId trackEntId) {
    for(auto& emitter : pool) {
        if(emitter->getEmitRequest().trackEntId == trackEntId) {
            emitter->stopEmitting();
            if(emitter->isFinished()) {
                updateFrameInfo.systemStopped += 1;
            }
        }
    }
}

void ParticlesEmittersPool::updateSubEmitterTm(int rootParticleId, const Transform& newTm) {
    assert(rootParticleId != InvalidRootParticleId && "Invalid root particle id");
    for(auto& emitter : pool) {
        if(emitter->getEmitRequest().rootParticleId == rootParticleId) {
            emitter->updateSubEmitterTm(newTm);
        }
    }
}

void ParticlesEmittersPool::stopEmitter(int rootParticleId) {
    assert(rootParticleId != InvalidRootParticleId && "Invalid root particle id");
    for(auto& emitter : pool) {
        if(emitter->getEmitRequest().rootParticleId == rootParticleId) {
            emitter->stopEmitting();
            if(emitter->isFinished()) {
                updateFrameInfo.systemStopped += 1;
            }
        }
    }
}

void ParticlesEmittersPool::simulate(const Transform& systemTm, float dt) {
    if(emittingState == EmittingState::Stopped) {
        return;
    }

    bool hasEmitting = false;
    bool hasAlive = false;

    for(auto& emitter : pool) {
        if(!emitter->isFinished()) {
            emitter->simulate(systemTm, dt);
        }
        if(emitter->isEmitting()) {
            hasEmitting = true;
        }
        if(!emitter->isFinished()) {
            hasAlive = true;
        } else {
            updateFrameInfo.systemStopped += 1;
        }
    }

    if(!hasEmitting) {
        if(hasAlive) {
            emittingState = EmittingState::Stopping;
        } else {
            emittingState = EmittingState::Stopped;
        }
    }
}

bool ParticlesEmittersPool::hasAlive() const {
   for(auto& emitter : pool) {
        if(!emitter->isFinished()) {
            return true;
        }
    }
    return false;
}

void ParticlesEmittersPool::stopEmittingAll() {
    if(emittingState != EmittingState::Playing) {
        return;
    }

    emittingState = EmittingState::Stopping;

    bool hasAlive = false;
    for(auto& emitter : pool) {
        if(!emitter->isFinished()) {
            hasAlive = true;
            emitter->stopEmitting();
        }
    }

    if(!hasAlive) {
        emittingState = EmittingState::Stopped;
        return;
    }
}

void ParticlesEmittersPool::destroyAll() {
    if(emittingState == EmittingState::Stopped) {
        return;
    }

    emittingState = EmittingState::Stopped;
    for(auto& emitter : pool) {
        if(!emitter->isFinished()) {
            emitter->stop();
            updateFrameInfo.systemStopped += 1;
        }
    }
}

SimulationConfig& ParticlesEmittersPool::getSimConfig() {
    return simConfig;
}

const SimulationConfig& ParticlesEmittersPool::getSimConfig() const {
    return simConfig;
}

int ParticlesEmittersPool::addParticles(int count) {
    auto maxParticles = Core::GetGlobal<RenderConfig>()->particlesConfig.maxParticles;
    auto resCount = std::min(maxParticles, count + particlesCount) - particlesCount;
    particlesCount += resCount;
    updateFrameInfo.particlesSpawned += count;
    return resCount;
}

void ParticlesEmittersPool::removeParticles(int count) {
    particlesCount -= count;
    updateFrameInfo.particlesRemoved += count;
    assert(particlesCount >= 0 && "Invalid particles count");
}

int ParticlesEmittersPool::getParticlesCount() const {
    return particlesCount;
}

ParticlesUpdateFrameInfo& ParticlesEmittersPool::getUpdateInfo() {
    return updateFrameInfo;
}

ParticlesUpdateFrameInfo ParticlesEmittersPool::getAndResetUpdateInfo() {
    auto res = updateFrameInfo;
    updateFrameInfo.reset();
    return res;
}

bool ParticlesEmittersPool::isStopped() const {
    return emittingState == EmittingState::Stopped;
}