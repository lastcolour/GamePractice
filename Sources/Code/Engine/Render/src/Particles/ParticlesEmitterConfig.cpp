#include "Render/ParticlesEmitterConfig.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Reflect/EnumInfo.hpp"

void ParticlesEmitterEmissionConfig::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<EmitterType>("EmitterType")) {
        enumInfo->addValues<EmitterType>({
            {"Sphere", EmitterType::Sphere},
            {"Box", EmitterType::Box}
        });
    }
    if(auto enumInfo = ctx.enumInfo<EmitterSpace>("EmitterSpace")) {
        enumInfo->addValues<EmitterSpace>({
            {"Local", EmitterSpace::Local},
            {"World", EmitterSpace::World}
        });
    }
    if(auto classInfo = ctx.classInfo<ParticlesEmitterEmissionConfig>("ParticlesEmitterEmissionConfig")) {
        classInfo->addField("duration", &ParticlesEmitterEmissionConfig::duration);
        classInfo->addField("autoStart", &ParticlesEmitterEmissionConfig::autoStart);
        classInfo->addField("loop", &ParticlesEmitterEmissionConfig::loop);
        classInfo->addField("startDelay", &ParticlesEmitterEmissionConfig::startDelay);
        classInfo->addField("emitterSpace", &ParticlesEmitterEmissionConfig::emitterSpace);
        classInfo->addField("emitterType", &ParticlesEmitterEmissionConfig::emitterType);
        classInfo->addField("emitterVal", &ParticlesEmitterEmissionConfig::emitterVal);
        classInfo->addField("thickness", &ParticlesEmitterEmissionConfig::thickness);
        classInfo->addField("emissionRate", &ParticlesEmitterEmissionConfig::emissionRate);
        classInfo->addField("direction", &ParticlesEmitterEmissionConfig::direction);
        classInfo->addField("directionVar", &ParticlesEmitterEmissionConfig::directionVar);
        classInfo->addField("lifetime", &ParticlesEmitterEmissionConfig::lifetime);
        classInfo->addField("lifetimeVar", &ParticlesEmitterEmissionConfig::lifetimeVar);
    }
}

ParticlesEmitterEmissionConfig::ParticlesEmitterEmissionConfig() :
    emitterType(EmitterType::Sphere),
    emitterSpace(EmitterSpace::World),
    emitterVal(1.f, 0.f),
    duration(4.f),
    thickness(0.f),
    direction(0.f),
    directionVar(0.1f),
    emissionRate(64.0f),
    lifetime(1.f),
    lifetimeVar(0.1f),
    startDelay(0.f),
    autoStart(true),
    loop(true) {
}

void ParticlesEmitterColorConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ParticlesEmitterColorConfig>("ParticlesEmitterColorConfig")) {
        classInfo->addField("startCol", &ParticlesEmitterColorConfig::startCol);
        classInfo->addField("startColVar", &ParticlesEmitterColorConfig::startColVar);
        classInfo->addField("endCol", &ParticlesEmitterColorConfig::endCol);
        classInfo->addField("endColVar", &ParticlesEmitterColorConfig::endColVar);
        classInfo->addField("fadeIn", &ParticlesEmitterColorConfig::fadeIn);
        classInfo->addField("fadeOut", &ParticlesEmitterColorConfig::fadeOut);
    }
}

ParticlesEmitterColorConfig::ParticlesEmitterColorConfig() :
    startCol(255, 255, 255),
    startColVar(0.1f),
    endCol(255, 0, 0),
    endColVar(0.1f),
    fadeIn(0.1f),
    fadeOut(0.1f) {
}

void ParticlesEmitterMovementConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ParticlesEmitterMovementConfig>("ParticlesEmitterMovementConfig")) {
        classInfo->addField("startScale", &ParticlesEmitterMovementConfig::startScale);
        classInfo->addField("startScaleVar", &ParticlesEmitterMovementConfig::startScaleVar);
        classInfo->addField("endScale", &ParticlesEmitterMovementConfig::endScale);
        classInfo->addField("endScaleVar", &ParticlesEmitterMovementConfig::endScaleVar);
        classInfo->addField("startSpeed", &ParticlesEmitterMovementConfig::startSpeed);
        classInfo->addField("startSpeedVar", &ParticlesEmitterMovementConfig::startSpeedVar);
        classInfo->addField("endSpeed", &ParticlesEmitterMovementConfig::endSpeed);
        classInfo->addField("endSpeedVar", &ParticlesEmitterMovementConfig::endSpeedVar);
        classInfo->addField("startRotation", &ParticlesEmitterMovementConfig::startRotation);
        classInfo->addField("startRotationVar", &ParticlesEmitterMovementConfig::startRotationVar);
        classInfo->addField("startRotSpeed", &ParticlesEmitterMovementConfig::startRotSpeed);
        classInfo->addField("startRotSpeedVar", &ParticlesEmitterMovementConfig::startRotSpeedVar);
        classInfo->addField("endRotSpeed", &ParticlesEmitterMovementConfig::endRotSpeed);
        classInfo->addField("endRotSpeedVar", &ParticlesEmitterMovementConfig::endRotSpeedVar);
    }
}

ParticlesEmitterMovementConfig::ParticlesEmitterMovementConfig() :
    startScale(1.f),
    startScaleVar(0.1f),
    endScale(1.5f),
    endScaleVar(0.1f),
    startSpeed(1.f),
    startSpeedVar(0.1f),
    endSpeed(2.f),
    endSpeedVar(0.1f),
    startRotation(0.f),
    startRotationVar(0.1f),
    startRotSpeed(30.f),
    startRotSpeedVar(0.1f),
    endRotSpeed(30.f),
    endRotSpeedVar(0.1f) {
}

void ParticlesEmitterRenderConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ParticlesEmitterRenderConfig>("ParticlesEmitterRenderConfig")) {
        classInfo->addResourceField("texture", ResourceType::Image, &ParticlesEmitterRenderConfig::texture);
    }
}

ParticlesEmitterRenderConfig::ParticlesEmitterRenderConfig() :
    texture("") {
}

void GravityField::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<GravityType>("GravityType")) {
        enumInfo->addValues<GravityType>({
            {"Simple", GravityType::Simple},
            {"Cicle", GravityType::Circle},
            {"Vortex", GravityType::Vortex}
        });
    }
    if(auto classInfo = ctx.classInfo<GravityField>("GravityField")) {
        classInfo->addField("type", &GravityField::type);
        classInfo->addField("offset", &GravityField::offset);
        classInfo->addField("dir", &GravityField::dir);
        classInfo->addField("value", &GravityField::value);
        classInfo->addField("radius", &GravityField::radius);
    }
}

GravityField::GravityField() :
    type(GravityType::Simple),
    offset(0.f),
    dir(-90.f),
    value(5.f),
    radius(-1.f) {
}

void ParticlesEmitterGravityConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ParticlesEmitterGravityConfig>("ParticlesEmitterGravityConfig")) {
        classInfo->addField("fields", &ParticlesEmitterGravityConfig::fields);
    }
}

void SubEmitter::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<SubEmitterTriggerEvent>("SubEmitterTriggerEvent")) {
        enumInfo->addValues<SubEmitterTriggerEvent>({
            {"OnStart", SubEmitterTriggerEvent::OnStart},
            {"OnEnd", SubEmitterTriggerEvent::OnEnd},
            {"OnParticleSpawn", SubEmitterTriggerEvent::OnParticleSpawn},
            {"OnParticleLive", SubEmitterTriggerEvent::OnParticleLive},
            {"OnParticleDeath", SubEmitterTriggerEvent::OnParticleDeath}
        });
    }
    if(auto classInfo = ctx.classInfo<SubEmitter>("SubEmitter")) {
        classInfo->addField("event", &SubEmitter::event);
        classInfo->addField("entityId", &SubEmitter::entId);
    }
}

SubEmitter::SubEmitter() :
    event(SubEmitterTriggerEvent::OnParticleDeath) {
}

SubEmitter::SubEmitter(SubEmitterTriggerEvent triggerEvent, EntityId entityId) :
    event(triggerEvent),
    entId(entityId) {
}

void ParticlesSubEmittersConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ParticlesSubEmittersConfig>("ParticlesSubEmittersConfig")) {
        classInfo->addField("subEmitters", &ParticlesSubEmittersConfig::subEmitters);
    }
}

ParticlesSubEmittersConfig::ParticlesSubEmittersConfig() {
}