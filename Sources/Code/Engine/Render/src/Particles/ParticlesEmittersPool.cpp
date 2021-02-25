#include "Particles/ParticlesEmittersPool.hpp"

#include <cassert>

ParticlesEmittersPool::ParticlesEmittersPool() {
}

ParticlesEmittersPool::~ParticlesEmittersPool() {
}

const std::vector<std::unique_ptr<EmitterParticles>>& ParticlesEmittersPool::getEmitters() const {
    return pool;
}

void ParticlesEmittersPool::createEmitter(int rootParticleId, const Transform& tm) {
    EmitterParticles* stoppedEmitter = nullptr;
    for(auto& emitter : pool) {
        if(emitter->isFinished()) {
            stoppedEmitter = emitter.get();
        }
    }
    if(!stoppedEmitter) {
        stoppedEmitter = pool.emplace_back(new EmitterParticles).get();
    }
    asyncState.store(AsynState::Playing);
    stoppedEmitter->setRootParticleId(rootParticleId);
    stoppedEmitter->start(tm);
}

void ParticlesEmittersPool::updateEmitterPos(int rootParticleId, const Vec2& newPt) {
    assert(rootParticleId != InvalidRootParticleId && "Invalid root particle id");
    for(auto& emitter : pool) {
        if(emitter->getRootParticleId() == rootParticleId) {
            
        }
    }
}

void ParticlesEmittersPool::simulate(const SimulationConfig& simConfig, const Transform& systemTm, float dt) {
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
            Transform tm;
            emitter->simulate(simConfig, systemTm, dt);
        }
        if(emitter->isEmitting()) {
            hasEmitting = true;
        }
        if(!emitter->isFinished()) {
            hasAlive = true;
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