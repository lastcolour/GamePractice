#include "Logics/ParticleEmitter.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Nodes/ParticlesNode.hpp"
#include "Core/ETLogger.hpp"

#include <cassert>

namespace {

void updatePartciles(std::atomic<ParticleEmitter::State>& state, EmitterParticles& particles, float dt) {
    auto currState = state.load();
    switch(currState) {
        case ParticleEmitter::State::PlayRequested: {
            state.store(ParticleEmitter::State::Playing);
            if(particles.isEmitting()) {
                particles.stop();
            }
            particles.start();
            break;
        }
        case ParticleEmitter::State::Playing: {
            break;
        }
        case ParticleEmitter::State::StopRequested: {
            if(particles.hasAlive()) {
                particles.stopEmitting();
                state.store(ParticleEmitter::State::Stopping);
            } else {
                particles.stop();
                state.store(ParticleEmitter::State::Stopped);
                return;
            }
            break;
        }
        case ParticleEmitter::State::Stopping: {
            if(!particles.isFinished()) {
                break;
            } else {
                particles.stop();
                state.store(ParticleEmitter::State::Stopped);
                return;
            }
        }
        case ParticleEmitter::State::Stopped: {
            return;
        }
        default: {
            assert(false && "Invalid emiter state");
            return;
        }
    }

    Transform tm;
    particles.update(tm, dt);

    if(particles.isFinished()) {
        if(particles.emissionConfig.autoStart) {
            particles.start();
        } else {
            state.store(ParticleEmitter::State::Stopped);
        }
    }
}

} // namespace

void ParticleEmitter::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ParticleEmitter>("ParticleEmitter")) {
        classInfo->addBaseClass<RenderNode>();
        classInfo->addField("emission", &ParticleEmitter::emissionConfig);
        classInfo->addField("movement", &ParticleEmitter::movementConfig);
        classInfo->addField("color", &ParticleEmitter::colorConfig);
        classInfo->addField("render", &ParticleEmitter::renderConfig);
        classInfo->addField("gravity", &ParticleEmitter::gravityConfig);
    }
}

ParticleEmitter::ParticleEmitter() :
    RenderNode(RenderNodeType::ParticleEmmiter),
    state(State::Stopped),
    canUpdate(false),
    isEmissionConfigChanged(true),
    isMovementConfigChanged(true),
    isColorConfigChanged(true),
    isRenderConfigChanged(true),
    isGravityConfigChanged(true) {
}

ParticleEmitter::~ParticleEmitter() {
}

void ParticleEmitter::init() {
    RenderNode::init();

    if(!proxyNode) {
        return;
    }

    ETNode<ETParticleEmitter>::connect(getEntityId());

    emissionConfig.lifetime = std::max(emissionConfig.lifetime, 0.01f);
    emissionConfig.emissionRate = std::max(emissionConfig.emissionRate, 0.01f);
    emissionConfig.startDelay = std::max(emissionConfig.startDelay, 0.f);

    isEmissionConfigChanged = true;
    isMovementConfigChanged = true;
    isColorConfigChanged = true;
    isRenderConfigChanged = true;
    isGravityConfigChanged = true;

    canUpdate = false;
    state.store(State::Stopped);
}

void ParticleEmitter::ET_setColorConfig(const ParticleEmitterColorConfig& newColorConf) {
    colorConfig = newColorConf;
    isColorConfigChanged = true;
}

const ParticleEmitterColorConfig& ParticleEmitter::ET_getColorConfig() const {
    return colorConfig;
}

void ParticleEmitter::ET_setMovementConfig(const ParticleEmitterMovementConfig& newMovementConf) {
    movementConfig = newMovementConf;
    isMovementConfigChanged = true;
}

const ParticleEmitterMovementConfig& ParticleEmitter::ET_getMovementConfig() const {
    return movementConfig;
}

void ParticleEmitter::ET_setEmissionConfig(const ParticleEmitterEmissionConfig& newEmissionConf) {
    emissionConfig = newEmissionConf;
    isEmissionConfigChanged = true;
}

const ParticleEmitterEmissionConfig& ParticleEmitter::ET_getEmissionConfig() const {
    return emissionConfig;
}

void ParticleEmitter::ET_setRenderConfig(const ParticleEmitterRenderConfig& newRenerConf) {
    renderConfig = newRenerConf;
    isRenderConfigChanged = true;
}

const ParticleEmitterRenderConfig& ParticleEmitter::ET_getRenderConfig() const {
    return renderConfig;
}

void ParticleEmitter::ET_emit() {
    if(!ET_isVisible()) {
        LogWarning("[ParticleEmitter::ET_emit] Can't emit hidden particle effect: '%s'",
            EntityUtils::GetEntityName(getEntityId()));
        return;
    }

    auto currState = state.load();
    if(currState == State::PlayRequested || state == State::Playing) {
        return;
    }

    state.store(State::PlayRequested);
}

bool ParticleEmitter::ET_isPlaying() const {
    return state.load() != State::Stopped;
}

void ParticleEmitter::onSyncWithRender() {
    bool restartEmitter = false;

    auto emitterProxyNode = static_cast<ParticlesNode*>(proxyNode);
    auto& particles = emitterProxyNode->getParticles();

    if(isEmissionConfigChanged) {
        particles.emissionConfig = emissionConfig;
        isEmissionConfigChanged = false;
    }
    if(isMovementConfigChanged) {
        particles.movementConfig = movementConfig;
        isMovementConfigChanged = false;
    }
    if(isColorConfigChanged) {
        isColorConfigChanged = false;
        particles.colorConfig = colorConfig;
    }
    if(isRenderConfigChanged) {
        restartEmitter = true;
        isRenderConfigChanged = false;
        emitterProxyNode->setConfig(renderConfig);
    }
    if(isGravityConfigChanged) {
        isGravityConfigChanged = false;
        particles.gravityConfig = gravityConfig;
    }

    if(!canUpdate) {
        canUpdate = true;
        ETNode<ETParticleUpdate>::connect(getEntityId());
        if(particles.emissionConfig.autoStart) {
            state.store(State::PlayRequested);
        }
    }

    if(restartEmitter) {
        if(ET_isPlaying()) {
           state.store(State::PlayRequested);
        }
    }
}

void ParticleEmitter::ET_updateEmitter(float dt) {
    auto emitterProxyNode = static_cast<ParticlesNode*>(proxyNode);
    auto& particles = emitterProxyNode->getParticles();

    updatePartciles(state, particles, dt);
}