#include "Logics/ParticlesSystem.hpp"
#include "Render/ETRenderTickManager.hpp"

#include <cassert>

void ParticlesSystem::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ParticlesSystem>("ParticlesSystem")) {
        classInfo->addBaseClass<DrawCommandProxy>();
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
    DrawCommandProxy(EDrawCmdType::Particles) {
}

ParticlesSystem::~ParticlesSystem() {
}

void ParticlesSystem::onInit() {
    emissionConfig.lifetime = std::max(emissionConfig.lifetime, 0.01f);
    emissionConfig.emissionRate = std::max(emissionConfig.emissionRate, 0.01f);
    emissionConfig.startDelay = std::max(emissionConfig.startDelay, 0.f);

    auto particlesCmd = static_cast<DrawParticlesCmd*>(cmd);
    particlesCmd->renderConfig = renderConfig;
    particlesCmd->tm = getTransform();

    const bool preMultAlpha = true; 
    particlesCmd->blendOpPair = RenderUtils::GetBlendOpPair(renderConfig.blendMode, preMultAlpha);

    auto& simConfig = particlesCmd->emittersPool.getSimConfig();
    simConfig.emission = emissionConfig;
    simConfig.movement = movementConfig;
    simConfig.color = colorConfig;
    simConfig.gravity = gravityConfig;
    simConfig.subEmittorsConfig = subEmittersConfig;
    simConfig.sizeConfig = sizeConfig;

    ETNode<ETParticlesSystem>::connect(getEntityId());
}

void ParticlesSystem::onLoaded() {
    DrawCommandProxy::onLoaded();
    if(emissionConfig.autoStart) {
        ET_emit();
    }
}

Mat4 ParticlesSystem::calcModelMat() const {
    return Mat4(1.f);
}

void ParticlesSystem::onTransformChanged(const Transform& newTm) {
    if(cmd) {
        Transform tmCopy = newTm;
        tmCopy.scale.x *= normScale;
        tmCopy.scale.y *= normScale;
        DrawParticlesCmd::QueueTMUpdate(*cmd, tmCopy);
    }
}

void ParticlesSystem::ET_setNormalizationScale(float newNormScale) {
    normScale = newNormScale;
    onTransformChanged(getTransform());

    if(emitEvents) {
        ET_SendEvent(getEntityId(), &ETRenderNodeEvents::ET_onNormScaleChanged, normScale);
    }
}

void ParticlesSystem::ET_setColorConfig(const ParticlesEmitterColorConfig& newColorConf) {
    colorConfig = newColorConf;
    DrawParticlesCmd::QueueColorConfigUpdate(*cmd, colorConfig);
}

const ParticlesEmitterColorConfig& ParticlesSystem::ET_getColorConfig() const {
    return colorConfig;
}

void ParticlesSystem::ET_setMovementConfig(const ParticlesEmitterMovementConfig& newMovementConf) {
    movementConfig = newMovementConf;
    DrawParticlesCmd::QueueMovementConfigUpdate(*cmd, movementConfig);
}

const ParticlesEmitterMovementConfig& ParticlesSystem::ET_getMovementConfig() const {
    return movementConfig;
}

void ParticlesSystem::ET_setEmissionConfig(const ParticlesEmitterEmissionConfig& newEmissionConf) {
    emissionConfig = newEmissionConf;
    DrawParticlesCmd::QueueEmissionConfigUpdate(*cmd, emissionConfig);
}

const ParticlesEmitterEmissionConfig& ParticlesSystem::ET_getEmissionConfig() const {
    return emissionConfig;
}

void ParticlesSystem::ET_setRenderConfig(const ParticlesEmitterRenderConfig& newRenerConf) {
    renderConfig = newRenerConf;
    DrawParticlesCmd::QueueRenderConfigUpdate(*cmd, renderConfig);
}

const ParticlesEmitterRenderConfig& ParticlesSystem::ET_getRenderConfig() const {
    return renderConfig;
}

void ParticlesSystem::ET_setSubEmittersConfig(const ParticlesSubEmittersConfig& newSubEmittersConf) {
    subEmittersConfig = newSubEmittersConf;
    DrawParticlesCmd::QueueSubEmittersConfigUpdate(*cmd, subEmittersConfig);
}

void ParticlesSystem::ET_setSizeConfig(const ParticlesEmitterSizeConfig& newSizeConf) {
    sizeConfig = newSizeConf;
    DrawParticlesCmd::QueueSizeConfigUpdate(*cmd, sizeConfig);
}

const ParticlesEmitterSizeConfig& ParticlesSystem::ET_getSizeConfig() const {
    return sizeConfig;
}

const ParticlesSubEmittersConfig& ParticlesSystem::ET_getSubEmittersConfig() const {
    return subEmittersConfig;
}

bool ParticlesSystem::ET_hasAliveParticles() const {
    auto particlesCmd = static_cast<DrawParticlesCmd*>(cmd);
    return !particlesCmd->emittersPool.isStopped();
}

void ParticlesSystem::ET_emit() {
    if(!ET_isVisible()) {
        LogWarning("[ParticlesSystem::ET_emit] Can't emit hidden particle effect: '%s'",
           getEntityName());
        return;
    }

    EmitRequest emitReq;
    emitReq.tmTrackType = EParticleTMTrackType::System;
    emitReq.rootParticleId = InvalidRootParticleId;

    DrawParticlesCmd::QueueCreateEmitterUpdate(*cmd, emitReq);
}

void ParticlesSystem::ET_emitWithTm(const Transform& emitTm) {
    if(!ET_isVisible()) {
        LogWarning("[ParticlesSystem::ET_emitWithTm] Can't emit hidden particle effect: '%s'",
           getEntityName());
        return;
    }

    EmitRequest emitReq;
    emitReq.rootParticleId = InvalidRootParticleId;
    emitReq.tmTrackType = EParticleTMTrackType::None;
    emitReq.tm = emitTm;

    DrawParticlesCmd::QueueCreateEmitterUpdate(*cmd, emitReq);
}

void ParticlesSystem::ET_emitTrackingEntity(EntityId trackEntId) {
    if(!ET_isVisible()) {
        LogWarning("[ParticlesSystem::ET_emitTrackingEntity] Can't emit hidden particle effect: '%s'",
            getEntityName());
        return;
    }
    if(!trackEntId.isValid()) {
        LogWarning("[ParticlesSystem::ET_emitTrackingEntity] Can't emit system '%s' to track invalid entity",
            getEntityName());
        return;
    }

    EmitRequest emitReq;
    emitReq.rootParticleId = InvalidRootParticleId;
    emitReq.tmTrackType = EParticleTMTrackType::Entity;
    emitReq.trackEntId = trackEntId;
    ET_SendEventReturn(emitReq.tm, trackEntId, &ETEntity::ET_getTransform);

    DrawParticlesCmd::QueueCreateEmitterUpdate(*cmd, emitReq);
}

void ParticlesSystem::ET_stopTrackedEmitter(EntityId trackEntId) {
    if(!trackEntId.isValid()) {
        LogWarning("[ParticlesSystem::ET_stopTrackedEmitter] Can't stop emit system '%s' with tracking invalid entity",
            getEntityName());
        return;
    }

    DrawParticlesCmd::QueueStopTrackedEmitterUpdate(*cmd, trackEntId);
}

void ParticlesSystem::ET_stopEmitting() {
    bool force = false;
    DrawParticlesCmd::QueueStopEmitting(*cmd, force);
}

void ParticlesSystem::ET_removeAll() {
    bool force = true;
    DrawParticlesCmd::QueueStopEmitting(*cmd, force);
}

void ParticlesSystem::ET_spawnSubEmitter(int rootParticleId, const Transform& spawnTm) {
    EmitRequest emitReq;
    emitReq.rootParticleId = rootParticleId;
    emitReq.tmTrackType = EParticleTMTrackType::None;
    emitReq.tm = spawnTm;

    auto particlesCmd = static_cast<DrawParticlesCmd*>(cmd);
    particlesCmd->emittersPool.createEmitter(emitReq);
}

void ParticlesSystem::ET_updateSubEmitter(int rootParticleId, const Transform& newTm) {
    auto particlesCmd = static_cast<DrawParticlesCmd*>(cmd);
    particlesCmd->emittersPool.updateSubEmitterTm(rootParticleId, newTm);
}

void ParticlesSystem::ET_stopSubEmitter(int rootParticleId) {
    auto particlesCmd = static_cast<DrawParticlesCmd*>(cmd);
    particlesCmd->emittersPool.stopEmitter(rootParticleId);
}