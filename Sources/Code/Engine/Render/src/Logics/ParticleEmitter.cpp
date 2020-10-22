#include "Logics/ParticleEmitter.hpp"
#include "Reflect/ReflectContext.hpp"

void ParticleEmitter::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ParticleEmitter>("ParticleEmitter")) {
        classInfo->addBaseClass<RenderNode>();
        classInfo->addField("duration", &ParticleEmitter::duration);
        classInfo->addField("particleEmissionRate", &ParticleEmitter::particleEmissionRate);
        classInfo->addField("particleLifetime", &ParticleEmitter::particleLifetime);
        classInfo->addField("particleSpeed", &ParticleEmitter::particleSpeed);
    }
}

ParticleEmitter::ParticleEmitter() :
    RenderNode(RenderNodeType::ParticleEmmiter) {
}

ParticleEmitter::~ParticleEmitter() {
}

bool ParticleEmitter::init() {
    RenderNode::init();
    ETNode<ETParticleEmitter>::connect(getEntityId());
    return true;
}

void ParticleEmitter::ET_emit() {
}