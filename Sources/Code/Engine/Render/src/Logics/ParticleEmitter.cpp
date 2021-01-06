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
        classInfo->addField("gravity", &ParticleEmitter::gravityConfig);
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

    emissionConfig.lifetime = std::max(emissionConfig.lifetime, 0.01f);
    emissionConfig.emissionRate = std::max(emissionConfig.emissionRate, 0.01f);
    emissionConfig.startDelay = std::max(emissionConfig.startDelay, 0.f);

    isConfigChanged = true;
}

void ParticleEmitter::ET_emit() {
}

void ParticleEmitter::onSyncWithRender() {
    auto emitterProxyNode = static_cast<ParticlesNode*>(proxyNode);
    if(isConfigChanged) {
        isConfigChanged = false;
        emitterProxyNode->setConfig(emissionConfig, movementConfig, colorConfig, renderConfig, gravityConfig);
    }
}