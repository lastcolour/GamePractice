#include "Logics/ParticlesSystem.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Nodes/ParticlesNode.hpp"
#include "Core/ETLogger.hpp"

#include <cassert>

void ParticlesSystem::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ParticlesSystem>("ParticlesSystem")) {
        classInfo->addBaseClass<RenderNode>();
        classInfo->addField("emission", &ParticlesSystem::emissionConfig);
        classInfo->addField("movement", &ParticlesSystem::movementConfig);
        classInfo->addField("size", &ParticlesSystem::sizeConfig);
        classInfo->addField("color", &ParticlesSystem::colorConfig);
        classInfo->addField("render", &ParticlesSystem::renderConfig);
        classInfo->addField("subEmitters", &ParticlesSystem::subEmittersConfig);
        classInfo->addField("gravity", &ParticlesSystem::gravityConfig);
    }
}

ParticlesSystem::ParticlesSystem() :
    RenderNode(RenderNodeType::ParticleEmmiter),
    canUpdate(false),
    isEmissionConfigChanged(true),
    isMovementConfigChanged(true),
    isColorConfigChanged(true),
    isRenderConfigChanged(true),
    isGravityConfigChanged(true),
    isSubEmittersConfigChanged(true) {
}

ParticlesSystem::~ParticlesSystem() {
}

void ParticlesSystem::init() {
    RenderNode::init();

    if(!proxyNode) {
        return;
    }

    ETNode<ETParticlesSystem>::connect(getEntityId());

    emissionConfig.lifetime = std::max(emissionConfig.lifetime, 0.01f);
    emissionConfig.emissionRate = std::max(emissionConfig.emissionRate, 0.01f);
    emissionConfig.startDelay = std::max(emissionConfig.startDelay, 0.f);

    isEmissionConfigChanged = true;
    isMovementConfigChanged = true;
    isColorConfigChanged = true;
    isRenderConfigChanged = true;
    isGravityConfigChanged = true;
    isSubEmittersConfigChanged = true;
    isSizeConfigChanged = true;

    canUpdate = false;

    if(emissionConfig.autoStart) {
        ET_emit();
    }
}

void ParticlesSystem::ET_setColorConfig(const ParticlesEmitterColorConfig& newColorConf) {
    colorConfig = newColorConf;
    isColorConfigChanged = true;
    markForSyncWithRender();
}

const ParticlesEmitterColorConfig& ParticlesSystem::ET_getColorConfig() const {
    return colorConfig;
}

void ParticlesSystem::ET_setMovementConfig(const ParticlesEmitterMovementConfig& newMovementConf) {
    movementConfig = newMovementConf;
    isMovementConfigChanged = true;
    markForSyncWithRender();
}

const ParticlesEmitterMovementConfig& ParticlesSystem::ET_getMovementConfig() const {
    return movementConfig;
}

void ParticlesSystem::ET_setEmissionConfig(const ParticlesEmitterEmissionConfig& newEmissionConf) {
    emissionConfig = newEmissionConf;
    isEmissionConfigChanged = true;
    markForSyncWithRender();
}

const ParticlesEmitterEmissionConfig& ParticlesSystem::ET_getEmissionConfig() const {
    return emissionConfig;
}

void ParticlesSystem::ET_setRenderConfig(const ParticlesEmitterRenderConfig& newRenerConf) {
    renderConfig = newRenerConf;
    isRenderConfigChanged = true;
    markForSyncWithRender();
}

const ParticlesEmitterRenderConfig& ParticlesSystem::ET_getRenderConfig() const {
    return renderConfig;
}

void ParticlesSystem::ET_setSubEmittersConfig(const ParticlesSubEmittersConfig& newSubEmittersConf) {
    subEmittersConfig = newSubEmittersConf;
    isSubEmittersConfigChanged = true;
    markForSyncWithRender();
}

void ParticlesSystem::ET_setSizeConfig(const ParticlesEmitterSizeConfig& newSizeConf) {
    sizeConfig = newSizeConf;
    isSizeConfigChanged = true;
    markForSyncWithRender();
}

const ParticlesEmitterSizeConfig& ParticlesSystem::ET_getSizeConfig() const {
    return sizeConfig;
}

const ParticlesSubEmittersConfig& ParticlesSystem::ET_getSubEmittersConfig() const {
    return subEmittersConfig;
}

bool ParticlesSystem::ET_hasAliveParticles() const {
    if(!proxyNode) {
        return false;
    }
    auto particlesNode = static_cast<ParticlesNode*>(proxyNode);
    auto& pool = particlesNode->getEmittersPool();
    return pool.asyncHasAlive();
}

void ParticlesSystem::onSyncWithRender() {
    auto particlesNode = static_cast<ParticlesNode*>(proxyNode);
    auto& pool = particlesNode->getEmittersPool();

    if(isEmissionConfigChanged) {
        simConfig.emission = emissionConfig;
        isEmissionConfigChanged = false;
    }
    if(isMovementConfigChanged) {
        simConfig.movement = movementConfig;
        isMovementConfigChanged = false;
    }
    if(isColorConfigChanged) {
        isColorConfigChanged = false;
        simConfig.color = colorConfig;
    }
    if(isRenderConfigChanged) {
        isRenderConfigChanged = false;
        particlesNode->setConfig(renderConfig);
    }
    if(isGravityConfigChanged) {
        isGravityConfigChanged = false;
        simConfig.gravity = gravityConfig;
    }
    if(isSubEmittersConfigChanged) {
        isSubEmittersConfigChanged = false;
        simConfig.subEmittorsConfig = subEmittersConfig;
    }
    if(isSizeConfigChanged) {
        isSizeConfigChanged = false;
        simConfig.sizeConfig = sizeConfig;
    }
    if(!canUpdate) {
        canUpdate = true;
        ETNode<ETParticlesUpdate>::connect(getEntityId());
    }

    if(!pendingEmits.empty()) {
        for(auto& emitReq : pendingEmits) {
            pool.createEmitter(emitReq);
        }
        pendingEmits.clear();
    }
}

void ParticlesSystem::ET_updateEmitter(float dt) {
    if(!proxyNode) {
        return;
    }
    auto particlesNode = static_cast<ParticlesNode*>(proxyNode);
    auto& emittersPool = particlesNode->getEmittersPool();
    auto& tm = particlesNode->getTransform();
    emittersPool.simulate(simConfig, tm, dt);
    if(!emittersPool.hasAlive()) {
        if(simConfig.emission.loop) {
            EmitRequest emitReq;
            emitReq.rootParticleId = InvalidRootParticleId;
            emitReq.syncWithSystemTm = true;
            emittersPool.createEmitter(emitReq);
        }
    }
}

void ParticlesSystem::ET_emit() {
    if(!ET_isVisible()) {
        LogWarning("[ParticlesSystem::ET_emit] Can't emit hidden particle effect: '%s'",
            EntityUtils::GetEntityName(getEntityId()));
        return;
    }

    EmitRequest emitReq;
    emitReq.syncWithSystemTm = true;
    emitReq.rootParticleId = InvalidRootParticleId;

    pendingEmits.emplace_back(emitReq);
    markForSyncWithRender();
}

void ParticlesSystem::ET_emitWithTm(const Transform& emitTm) {
    if(!ET_isVisible()) {
        LogWarning("[ParticlesSystem::ET_emitWithTm] Can't emit hidden particle effect: '%s'",
            EntityUtils::GetEntityName(getEntityId()));
        return;
    }

    EmitRequest emitReq;
    emitReq.rootParticleId = InvalidRootParticleId;
    emitReq.syncWithSystemTm = false;
    emitReq.tm = emitTm;

    pendingEmits.emplace_back(emitReq);
    markForSyncWithRender();
}

void ParticlesSystem::ET_stopEmitting() {
    pendingEmits.clear();
    if(!proxyNode) {
        return;
    }
    auto particlesNode = static_cast<ParticlesNode*>(proxyNode);
    auto& emittersPool = particlesNode->getEmittersPool();
    emittersPool.asyncStopEmitting();
}

void ParticlesSystem::ET_destoryAll() {
    pendingEmits.clear();
    if(!proxyNode) {
        return;
    }
    auto particlesNode = static_cast<ParticlesNode*>(proxyNode);
    auto& emittersPool = particlesNode->getEmittersPool();
    emittersPool.asyncDestroyAll();
}

void ParticlesSystem::ET_spawnSubEmitter(int rootParticleId, const Transform& spawnTm) {
    if(!proxyNode) {
        return;
    }
    auto particlesNode = static_cast<ParticlesNode*>(proxyNode);
    auto& emittersPool = particlesNode->getEmittersPool();

    EmitRequest emitReq;
    emitReq.rootParticleId = rootParticleId;
    emitReq.syncWithSystemTm = false;
    emitReq.tm = spawnTm;

    emittersPool.createEmitter(emitReq);
}

void ParticlesSystem::ET_updateSubEmitter(int rootParticleId, const Vec2& pt) {
    if(!proxyNode) {
        return;
    }
    auto particlesNode = static_cast<ParticlesNode*>(proxyNode);
    auto& emittersPool = particlesNode->getEmittersPool();
    emittersPool.updateEmitterPos(rootParticleId, pt);
}

void ParticlesSystem::ET_stopSubEmitter(int rootParticleId) {
    if(!proxyNode) {
        return;
    }
    auto particlesNode = static_cast<ParticlesNode*>(proxyNode);
    auto& emittersPool = particlesNode->getEmittersPool();
    emittersPool.stopEmitter(rootParticleId);
}