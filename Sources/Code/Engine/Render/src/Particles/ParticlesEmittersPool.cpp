#include "Particles/ParticlesEmittersPool.hpp"
#include "Core/GlobalData.hpp"
#include "RenderConfig.hpp"

#include <cassert>

ParticlesEmittersPool::ParticlesEmittersPool() :
    asyncState(AsynState::Stopped),
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
    asyncState.store(AsynState::Playing);
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
    auto currState = asyncState.load();
    switch(currState) {
        case AsynState::Playing: {
            break;
        }
        case AsynState::StopRequested: {
            bool hasAlive = false;
            asyncState.store(AsynState::Stoppig);
            for(auto& emitter : pool) {
                if(!emitter->isFinished()) {
                    hasAlive = true;
                    emitter->stopEmitting();
                }
            }
            if(!hasAlive) {
                asyncState.store(AsynState::Stopped);
                return;
            }
            break;
        }
        case AsynState::ForceStopRequested: {
            asyncState.store(AsynState::Stopped);
            for(auto& emitter : pool) {
                if(!emitter->isFinished()) {
                    emitter->stop();
                    updateFrameInfo.systemStopped += 1;
                }
            }
            return;
        }
        case AsynState::Stoppig: {
            break;
        }
        case AsynState::Stopped: {
            return;
        }
        default: {
            assert(false && "Invalid async state");
        }
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
            asyncState.store(AsynState::Stoppig);
        } else {
            asyncState.store(AsynState::Stopped);
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

void ParticlesEmittersPool::asyncStopEmitting() {
    auto currState = asyncState.load();
    if(currState != AsynState::Playing) {
        return;
    }
    asyncState.store(AsynState::StopRequested);
}

void ParticlesEmittersPool::asyncDestroyAll() {
    auto currState = asyncState.load();
    if(currState == AsynState::Stopped) {
        return;
    }
    asyncState.store(AsynState::ForceStopRequested);
}

bool ParticlesEmittersPool::asyncHasAlive() const {
    auto currState = asyncState.load();
    return currState != AsynState::Stopped;
}

SimulationConfig& ParticlesEmittersPool::getSimConfig() {
    return simConfig;
}

const SimulationConfig& ParticlesEmittersPool::getSimConfig() const {
    return simConfig;
}

int ParticlesEmittersPool::addParticles(int count) {
    auto maxParticles = GetGlobal<RenderConfig>()->particlesConfig.maxParticles;
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

ParticlesUpdateFrameInfo& ParticlesEmittersPool::getUpdateInfo() {
    return updateFrameInfo;
}

ParticlesUpdateFrameInfo ParticlesEmittersPool::getAndResetUpdateInfo() {
    auto res = updateFrameInfo;
    updateFrameInfo.reset();
    return res;
}