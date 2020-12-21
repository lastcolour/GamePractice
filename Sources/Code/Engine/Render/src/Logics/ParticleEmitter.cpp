#include "Logics/ParticleEmitter.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Nodes/ParticlesNode.hpp"
#include "Core/ETLogger.hpp"

void ParticleEmitter::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ParticleEmitter>("ParticleEmitter")) {
        classInfo->addBaseClass<RenderNode>();
        classInfo->addField("emission", &ParticleEmitter::emissionConfig);
        classInfo->addField("movement", &ParticleEmitter::movementConfig);
        classInfo->addField("color", &ParticleEmitter::colorConfig);
        classInfo->addField("render", &ParticleEmitter::renderConfig);
    }
}

ParticleEmitter::ParticleEmitter() :
    RenderNode(RenderNodeType::ParticleEmmiter),
    isConfigChanged(true) {
}

ParticleEmitter::~ParticleEmitter() {
}

void ParticleEmitter::init() {
    RenderNode::init();
    ETNode<ETParticleEmitter>::connect(getEntityId());

    if(emissionConfig.lifetime <= 0.f) {
        LogWarning("[ParticlesNode::ET_setConfig] Particle lifetime is negative or zero");
        emissionConfig.lifetime = 0.01f;
    }
    if(emissionConfig.emissionRate <= 0.f) {
        LogWarning("[ParticlesNode::ET_setConfig] Emitter emission rate is negative or zero");
        emissionConfig.emissionRate = 1.f;
    }
    if(emissionConfig.startDelay < 0.f) {
        LogWarning("[ParticlesNode::ET_setConfig] Emitter start delay is negative");
        emissionConfig.startDelay = 0.f;
    }

    isConfigChanged = true;
}

void ParticleEmitter::ET_emit() {
}

void ParticleEmitter::onSyncWithRender() {
    auto emitterProxyNode = static_cast<ParticlesNode*>(proxyNode);
    if(isConfigChanged) {
        isConfigChanged = false;
        emitterProxyNode->setConfig(emissionConfig, movementConfig, colorConfig, renderConfig);
    }
}