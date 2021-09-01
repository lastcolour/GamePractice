#include "Logics/ParticlesSystem.hpp"
#include "Nodes/ParticlesNode.hpp"
#include "Render/ETRenderTickManager.hpp"

#include <cassert>

namespace {

float getUpdateDeltaTime(const ParticlesEmitterEmissionConfig& emissionConfig, float renderDt) {
    float dt = 0.f;
    switch(emissionConfig.emitterSimTime) {
        case EmitterSimulationTime::Render: {
            dt = renderDt;
            break;
        }
        case EmitterSimulationTime::Game: {
            ET_SendEventReturn(dt, &ETRenderTickManager::ET_getUIDeltaT);
            break;
        }
        case EmitterSimulationTime::UI: {
            ET_SendEventReturn(dt, &ETRenderTickManager::ET_getUIDeltaT);
            break;
        }
        default: {
            assert(false && "Invalid simulation time");
        }
    }
    return dt;
}

} // namespace

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
    RenderNode(RenderNodeType::ParticleEmmiter) {
}

ParticlesSystem::~ParticlesSystem() {
}

void ParticlesSystem::onInit() {
    emissionConfig.lifetime = std::max(emissionConfig.lifetime, 0.01f);
    emissionConfig.emissionRate = std::max(emissionConfig.emissionRate, 0.01f);
    emissionConfig.startDelay = std::max(emissionConfig.startDelay, 0.f);

    auto particlesNode = static_cast<ParticlesNode*>(proxyNode);
    auto& pool = particlesNode->getEmittersPool();
    auto& simConfig = pool.getSimConfig();

    simConfig.emission = emissionConfig;
    simConfig.movement = movementConfig;
    simConfig.color = colorConfig;
    simConfig.gravity = gravityConfig;
    simConfig.subEmittorsConfig = subEmittersConfig;
    simConfig.sizeConfig = sizeConfig;

    ET_QueueEvent(&ETRenderNodeManager::ET_scheduleNodeEvent, [node=particlesNode, config=renderConfig](){
        node->setConfig(config);
    });

    if(emissionConfig.autoStart) {
        ET_emit();
    }
    ETNode<ETParticlesSystem>::connect(getEntityId());
    ETNode<ETParticlesUpdate>::connect(getEntityId());
}

void ParticlesSystem::ET_setColorConfig(const ParticlesEmitterColorConfig& newColorConf) {
    colorConfig = newColorConf;
    ET_QueueEvent(&ETParticlesManager::ET_scheduleEmitterEvent, [node=proxyNode, newColorConf](){
        auto particlesNode = static_cast<ParticlesNode*>(node);
        auto& pool = particlesNode->getEmittersPool();
        auto& simConfig = pool.getSimConfig();
        simConfig.color = newColorConf;
    });
}

const ParticlesEmitterColorConfig& ParticlesSystem::ET_getColorConfig() const {
    return colorConfig;
}

void ParticlesSystem::ET_setMovementConfig(const ParticlesEmitterMovementConfig& newMovementConf) {
    movementConfig = newMovementConf;
    ET_QueueEvent(&ETParticlesManager::ET_scheduleEmitterEvent, [node=proxyNode, newMovementConf](){
        auto particlesNode = static_cast<ParticlesNode*>(node);
        auto& pool = particlesNode->getEmittersPool();
        auto& simConfig = pool.getSimConfig();
        simConfig.movement = newMovementConf;
    });
}

const ParticlesEmitterMovementConfig& ParticlesSystem::ET_getMovementConfig() const {
    return movementConfig;
}

void ParticlesSystem::ET_setEmissionConfig(const ParticlesEmitterEmissionConfig& newEmissionConf) {
    emissionConfig = newEmissionConf;
    ET_QueueEvent(&ETParticlesManager::ET_scheduleEmitterEvent, [node=proxyNode, newEmissionConf](){
        auto particlesNode = static_cast<ParticlesNode*>(node);
        auto& pool = particlesNode->getEmittersPool();
        auto& simConfig = pool.getSimConfig();
        simConfig.emission = newEmissionConf;
    });
}

const ParticlesEmitterEmissionConfig& ParticlesSystem::ET_getEmissionConfig() const {
    return emissionConfig;
}

void ParticlesSystem::ET_setRenderConfig(const ParticlesEmitterRenderConfig& newRenerConf) {
    renderConfig = newRenerConf;
    ET_QueueEvent(&ETRenderNodeManager::ET_scheduleNodeEvent, [node=proxyNode, newRenerConf](){
        auto particlesNode = static_cast<ParticlesNode*>(node);
        particlesNode->setConfig(newRenerConf);
    });
}

const ParticlesEmitterRenderConfig& ParticlesSystem::ET_getRenderConfig() const {
    return renderConfig;
}

void ParticlesSystem::ET_setSubEmittersConfig(const ParticlesSubEmittersConfig& newSubEmittersConf) {
    subEmittersConfig = newSubEmittersConf;
    ET_QueueEvent(&ETParticlesManager::ET_scheduleEmitterEvent, [node=proxyNode, newSubEmittersConf](){
        auto particlesNode = static_cast<ParticlesNode*>(node);
        auto& pool = particlesNode->getEmittersPool();
        auto& simConfig = pool.getSimConfig();
        simConfig.subEmittorsConfig = newSubEmittersConf;
    });
}

void ParticlesSystem::ET_setSizeConfig(const ParticlesEmitterSizeConfig& newSizeConf) {
    sizeConfig = newSizeConf;
    ET_QueueEvent(&ETParticlesManager::ET_scheduleEmitterEvent, [node=proxyNode, newSizeConf](){
        auto particlesNode = static_cast<ParticlesNode*>(node);
        auto& pool = particlesNode->getEmittersPool();
        auto& simConfig = pool.getSimConfig();
        simConfig.sizeConfig = newSizeConf;
    });
}

const ParticlesEmitterSizeConfig& ParticlesSystem::ET_getSizeConfig() const {
    return sizeConfig;
}

const ParticlesSubEmittersConfig& ParticlesSystem::ET_getSubEmittersConfig() const {
    return subEmittersConfig;
}

bool ParticlesSystem::ET_hasAliveParticles() const {
    auto particlesNode = static_cast<ParticlesNode*>(proxyNode);
    auto& pool = particlesNode->getEmittersPool();
    return pool.asyncHasAlive();
}

void ParticlesSystem::ET_updateEmitter(float dt) {
    auto particlesNode = static_cast<ParticlesNode*>(proxyNode);
    auto& pool = particlesNode->getEmittersPool();
    auto& tm = particlesNode->getTransform();
    auto& simConfig = pool.getSimConfig();
    auto deltaT = getUpdateDeltaTime(simConfig.emission, dt);

    pool.simulate(tm, deltaT);

    auto updateInfo = pool.getAndResetUpdateInfo();
    DispatchParticlesSystemEvents(getEntityId(), updateInfo);
}

void ParticlesSystem::ET_emit() {
    if(!ET_isVisible()) {
        LogWarning("[ParticlesSystem::ET_emit] Can't emit hidden particle effect: '%s'",
            EntityUtils::GetEntityName(getEntityId()));
        return;
    }

    EmitRequest emitReq;
    emitReq.tmTrackType = EParticleTMTrackType::System;
    emitReq.rootParticleId = InvalidRootParticleId;

    ET_QueueEvent(&ETParticlesManager::ET_scheduleEmitterEvent, [node=proxyNode, emitReq](){
        auto particlesNode = static_cast<ParticlesNode*>(node);
        auto& pool = particlesNode->getEmittersPool();
        pool.createEmitter(emitReq);
    });
}

void ParticlesSystem::ET_emitWithTm(const Transform& emitTm) {
    if(!ET_isVisible()) {
        LogWarning("[ParticlesSystem::ET_emitWithTm] Can't emit hidden particle effect: '%s'",
            EntityUtils::GetEntityName(getEntityId()));
        return;
    }

    EmitRequest emitReq;
    emitReq.rootParticleId = InvalidRootParticleId;
    emitReq.tmTrackType = EParticleTMTrackType::None;
    emitReq.tm = emitTm;

    ET_QueueEvent(&ETParticlesManager::ET_scheduleEmitterEvent, [node=proxyNode, emitReq](){
        auto particlesNode = static_cast<ParticlesNode*>(node);
        auto& pool = particlesNode->getEmittersPool();
        pool.createEmitter(emitReq);
    });
}

void ParticlesSystem::ET_emitTrackingEntity(EntityId trackEntId) {
    if(!ET_isVisible()) {
        LogWarning("[ParticlesSystem::ET_emitTrackingEntity] Can't emit hidden particle effect: '%s'",
            EntityUtils::GetEntityName(getEntityId()));
        return;
    }
    if(!trackEntId.isValid()) {
        LogWarning("[ParticlesSystem::ET_emitTrackingEntity] Can't emit system '%s' to track invalid entity",
            EntityUtils::GetEntityName(getEntityId()));
        return;
    }

    EmitRequest emitReq;
    emitReq.rootParticleId = InvalidRootParticleId;
    emitReq.tmTrackType = EParticleTMTrackType::Entity;
    emitReq.trackEntId = trackEntId;
    ET_SendEventReturn(emitReq.tm, trackEntId, &ETEntity::ET_getTransform);

    ET_QueueEvent(&ETParticlesManager::ET_scheduleEmitterEvent, [node=proxyNode, emitReq](){
        auto particlesNode = static_cast<ParticlesNode*>(node);
        auto& pool = particlesNode->getEmittersPool();
        pool.createEmitter(emitReq);
    });
}

void ParticlesSystem::ET_stopTrackedEmitter(EntityId trackEntId) {
    if(!trackEntId.isValid()) {
        LogWarning("[ParticlesSystem::ET_stopTrackedEmitter] Can't stop emit system '%s' with tracking invalid entity",
            EntityUtils::GetEntityName(getEntityId()));
        return;
    }

    ET_QueueEvent(&ETParticlesManager::ET_scheduleEmitterEvent, [node=proxyNode, trackEntId](){
        auto particlesNode = static_cast<ParticlesNode*>(node);
        auto& pool = particlesNode->getEmittersPool();
        pool.stopTrackEmitter(trackEntId);
    });
}

void ParticlesSystem::ET_stopEmitting() {
    auto particlesNode = static_cast<ParticlesNode*>(proxyNode);
    auto& emittersPool = particlesNode->getEmittersPool();
    emittersPool.asyncStopEmitting();
}

void ParticlesSystem::ET_removeAll() {
    auto particlesNode = static_cast<ParticlesNode*>(proxyNode);
    auto& emittersPool = particlesNode->getEmittersPool();
    emittersPool.asyncDestroyAll();
}

void ParticlesSystem::ET_spawnSubEmitter(int rootParticleId, const Transform& spawnTm) {
    auto particlesNode = static_cast<ParticlesNode*>(proxyNode);
    auto& emittersPool = particlesNode->getEmittersPool();

    EmitRequest emitReq;
    emitReq.rootParticleId = rootParticleId;
    emitReq.tmTrackType = EParticleTMTrackType::None;
    emitReq.tm = spawnTm;

    emittersPool.createEmitter(emitReq);
}

void ParticlesSystem::ET_updateSubEmitter(int rootParticleId, const Transform& newTm) {
    auto particlesNode = static_cast<ParticlesNode*>(proxyNode);
    auto& emittersPool = particlesNode->getEmittersPool();
    emittersPool.updateSubEmitterTm(rootParticleId, newTm);
}

void ParticlesSystem::ET_stopSubEmitter(int rootParticleId) {
    auto particlesNode = static_cast<ParticlesNode*>(proxyNode);
    auto& emittersPool = particlesNode->getEmittersPool();
    emittersPool.stopEmitter(rootParticleId);
}