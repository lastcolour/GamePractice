#include "Render/ParticlesEmitterConfig.hpp"
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
    if(auto enumInfo = ctx.enumInfo<EmitterSimulationTime>("EmitterSimulationTime")) {
        enumInfo->addValues<EmitterSimulationTime>({
            {"Render", EmitterSimulationTime::Render},
            {"Game", EmitterSimulationTime::Game},
            {"UI", EmitterSimulationTime::UI}
        });
    }
    if(auto classInfo = ctx.classInfo<ParticlesEmitterEmissionConfig>("ParticlesEmitterEmissionConfig")) {
        classInfo->addField("simTime", &ParticlesEmitterEmissionConfig::emitterSimTime);
        classInfo->addField("duration", &ParticlesEmitterEmissionConfig::duration);
        classInfo->addField("startDelay", &ParticlesEmitterEmissionConfig::startDelay);
        classInfo->addField("autoStart", &ParticlesEmitterEmissionConfig::autoStart);
        classInfo->addField("loop", &ParticlesEmitterEmissionConfig::loop);
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
    emitterSimTime(EmitterSimulationTime::Render),
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
    autoStart(false),
    loop(false) {
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

void ParticlesEmitterSizeConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ParticlesEmitterSizeConfig>("ParticlesEmitterSizeConfig")) {
        classInfo->addField("startScale", &ParticlesEmitterSizeConfig::startScale);
        classInfo->addField("startScaleVar", &ParticlesEmitterSizeConfig::startScaleVar);
        classInfo->addField("endScale", &ParticlesEmitterSizeConfig::endScale);
        classInfo->addField("endScaleVar", &ParticlesEmitterSizeConfig::endScaleVar);
    }
}

ParticlesEmitterSizeConfig::ParticlesEmitterSizeConfig() :
    startScale(5.f),
    startScaleVar(0.1f),
    endScale(5.f),
    endScaleVar(0.1f) {
}

void ParticlesEmitterMovementConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ParticlesEmitterMovementConfig>("ParticlesEmitterMovementConfig")) {
        classInfo->addField("speed", &ParticlesEmitterMovementConfig::speed);
        classInfo->addField("speedVar", &ParticlesEmitterMovementConfig::speedVar);
        classInfo->addField("initRotation", &ParticlesEmitterMovementConfig::initRotation);
        classInfo->addField("initRotationVar", &ParticlesEmitterMovementConfig::initRotationVar);
        classInfo->addField("rotationSpeed", &ParticlesEmitterMovementConfig::rotationSpeed);
        classInfo->addField("rotationSpeedVar", &ParticlesEmitterMovementConfig::rotationSpeedVar);
        classInfo->addField("rotationAcc", &ParticlesEmitterMovementConfig::rotationAcc);
    }
}

ParticlesEmitterMovementConfig::ParticlesEmitterMovementConfig() :
    speed(100.f),
    speedVar(0.1f),
    initRotation(30.f),
    initRotationVar(0.1f),
    rotationSpeed(10.f),
    rotationSpeedVar(0.1f),
    rotationAcc(0.f) {
}

void ParticlesEmitterRenderConfig::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<EBlendMode>("EBlendMode")) {
        enumInfo->addValues<EBlendMode>({
            {"Normal", EBlendMode::Normal},
            {"Additive", EBlendMode::Additive},
            {"Multiply", EBlendMode::Multiply},
            {"Screen", EBlendMode::Screen}
        });
    }
    if(auto classInfo = ctx.classInfo<ParticlesEmitterRenderConfig>("ParticlesEmitterRenderConfig")) {
        classInfo->addField("blending", &ParticlesEmitterRenderConfig::blendMode);
        classInfo->addField("textureInfo", &ParticlesEmitterRenderConfig::textureInfo);
    }
}

ParticlesEmitterRenderConfig::ParticlesEmitterRenderConfig() :
    blendMode(EBlendMode::Normal) {
}

void GravityField::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<GravityType>("GravityType")) {
        enumInfo->addValues<GravityType>({
            {"Simple", GravityType::Simple},
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
    entId(entityId),
    event(triggerEvent) {
}

void ParticlesSubEmittersConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ParticlesSubEmittersConfig>("ParticlesSubEmittersConfig")) {
        classInfo->addField("subEmitters", &ParticlesSubEmittersConfig::subEmitters);
    }
}

ParticlesSubEmittersConfig::ParticlesSubEmittersConfig() {
}