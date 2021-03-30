#include "Logics/ParticlesSystem.hpp"
#include "Nodes/ParticlesNode.hpp"

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

    ET_QueueEvent(&ETRenderNodeManager::ET_addUpdateEvent, [node=particlesNode, config=renderConfig](){
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
    ET_QueueEvent(&ETRenderNodeManager::ET_addUpdateEvent, [node=proxyNode, newColorConf](){
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
    ET_QueueEvent(&ETRenderNodeManager::ET_addUpdateEvent, [node=proxyNode, newMovementConf](){
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
    ET_QueueEvent(&ETRenderNodeManager::ET_addUpdateEvent, [node=proxyNode, newEmissionConf](){
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
    ET_QueueEvent(&ETRenderNodeManager::ET_addUpdateEvent, [node=proxyNode, newRenerConf](){
        auto particlesNode = static_cast<ParticlesNode*>(node);
        particlesNode->setConfig(newRenerConf);
    });
}

const ParticlesEmitterRenderConfig& ParticlesSystem::ET_getRenderConfig() const {
    return renderConfig;
}

void ParticlesSystem::ET_setSubEmittersConfig(const ParticlesSubEmittersConfig& newSubEmittersConf) {
    subEmittersConfig = newSubEmittersConf;
    ET_QueueEvent(&ETRenderNodeManager::ET_addUpdateEvent, [node=proxyNode, newSubEmittersConf](){
        auto particlesNode = static_cast<ParticlesNode*>(node);
        auto& pool = particlesNode->getEmittersPool();
        auto& simConfig = pool.getSimConfig();
        simConfig.subEmittorsConfig = newSubEmittersConf;
    });
}

void ParticlesSystem::ET_setSizeConfig(const ParticlesEmitterSizeConfig& newSizeConf) {
    sizeConfig = newSizeConf;
    ET_QueueEvent(&ETRenderNodeManager::ET_addUpdateEvent, [node=proxyNode, newSizeConf](){
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
    pool.simulate(tm, dt);
    if(!pool.hasAlive()) {
        auto& simConfig = pool.getSimConfig();
        if(simConfig.emission.loop) {
            EmitRequest emitReq;
            emitReq.rootParticleId = InvalidRootParticleId;
            emitReq.syncWithSystemTm = true;
            pool.createEmitter(emitReq);
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

    ET_QueueEvent(&ETRenderNodeManager::ET_addUpdateEvent, [node=proxyNode, emitReq](){
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
    emitReq.syncWithSystemTm = false;
    emitReq.tm = emitTm;

    ET_QueueEvent(&ETRenderNodeManager::ET_addUpdateEvent, [node=proxyNode, emitReq](){
        auto particlesNode = static_cast<ParticlesNode*>(node);
        auto& pool = particlesNode->getEmittersPool();
        pool.createEmitter(emitReq);
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
    emitReq.syncWithSystemTm = false;
    emitReq.tm = spawnTm;

    emittersPool.createEmitter(emitReq);
}

void ParticlesSystem::ET_updateSubEmitter(int rootParticleId, const Vec2& pt) {
    auto particlesNode = static_cast<ParticlesNode*>(proxyNode);
    auto& emittersPool = particlesNode->getEmittersPool();
    emittersPool.updateEmitterPos(rootParticleId, pt);
}

void ParticlesSystem::ET_stopSubEmitter(int rootParticleId) {
    auto particlesNode = static_cast<ParticlesNode*>(proxyNode);
    auto& emittersPool = particlesNode->getEmittersPool();
    emittersPool.stopEmitter(rootParticleId);
}