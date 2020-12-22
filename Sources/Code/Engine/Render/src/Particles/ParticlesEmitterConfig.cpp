#include "Particles/ParticlesEmitterConfig.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Reflect/EnumInfo.hpp"

void ParticleEmitterEmissionConfig::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<EmitterType>("EmitterType")) {
        enumInfo->addValues<EmitterType>({
            {"Sphere", EmitterType::Sphere},
            {"Cone", EmitterType::Cone},
            {"Box", EmitterType::Box}
        });
    }
    if(auto enumInfo = ctx.enumInfo<EmitterSpace>("EmitterSpace")) {
        enumInfo->addValues<EmitterSpace>({
            {"Local", EmitterSpace::Local},
            {"World", EmitterSpace::World}
        });
    }
    if(auto classInfo = ctx.classInfo<ParticleEmitterEmissionConfig>("ParticleEmitterEmissionConfig")) {
        classInfo->addField("emitterSpace", &ParticleEmitterEmissionConfig::emitterSpace);
        classInfo->addField("emitterType", &ParticleEmitterEmissionConfig::emitterType);
        classInfo->addField("emitterVal", &ParticleEmitterEmissionConfig::emitterVal);
        classInfo->addField("duration", &ParticleEmitterEmissionConfig::duration);
        classInfo->addField("startEmissionRate", &ParticleEmitterEmissionConfig::startEmissionRate);
        classInfo->addField("endEmissionRate", &ParticleEmitterEmissionConfig::endEmissionRate);
        classInfo->addField("lifetime", &ParticleEmitterEmissionConfig::lifetime);
        classInfo->addField("startDelay", &ParticleEmitterEmissionConfig::startDelay);
        classInfo->addField("autoStart", &ParticleEmitterEmissionConfig::autoStart);
        classInfo->addField("loop", &ParticleEmitterEmissionConfig::loop);
    }
}

ParticleEmitterEmissionConfig::ParticleEmitterEmissionConfig() :
    emitterType(EmitterType::Sphere),
    emitterSpace(EmitterSpace::World),
    emitterVal(1.f, 0.f),
    duration(4.f),
    startEmissionRate(64.0f),
    endEmissionRate(128.0f),
    lifetime(1.f),
    startDelay(0.f),
    autoStart(true),
    loop(true) {
}

void ParticleEmitterColorConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ParticleEmitterColorConfig>("ParticleEmitterColorConfig")) {
        classInfo->addField("startCol", &ParticleEmitterColorConfig::startCol);
        classInfo->addField("startColVar", &ParticleEmitterColorConfig::startColVar);
        classInfo->addField("endCol", &ParticleEmitterColorConfig::endCol);
        classInfo->addField("endColVar", &ParticleEmitterColorConfig::endColVar);
    }
}

ParticleEmitterColorConfig::ParticleEmitterColorConfig() :
    startCol(255, 255, 255),
    startColVar(0.1f),
    endCol(255, 0, 0),
    endColVar(0.1f) {
}

void ParticleEmitterMovementConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ParticleEmitterMovementConfig>("ParticleEmitterMovementConfig")) {
        classInfo->addField("startScale", &ParticleEmitterMovementConfig::startScale);
        classInfo->addField("startScaleVar", &ParticleEmitterMovementConfig::startScaleVar);
        classInfo->addField("endScale", &ParticleEmitterMovementConfig::endScale);
        classInfo->addField("endScaleVar", &ParticleEmitterMovementConfig::endScaleVar);
        classInfo->addField("startSpeed", &ParticleEmitterMovementConfig::startSpeed);
        classInfo->addField("startSpeedVar", &ParticleEmitterMovementConfig::startSpeedVar);
        classInfo->addField("endSpeed", &ParticleEmitterMovementConfig::endSpeed);
        classInfo->addField("endSpeedVar", &ParticleEmitterMovementConfig::endSpeedVar);
        classInfo->addField("startRotation", &ParticleEmitterMovementConfig::startRotation);
        classInfo->addField("startRotationVar", &ParticleEmitterMovementConfig::startRotationVar);
        classInfo->addField("startRotSpeed", &ParticleEmitterMovementConfig::startRotSpeed);
        classInfo->addField("startRotSpeedVar", &ParticleEmitterMovementConfig::startRotSpeedVar);
        classInfo->addField("endRotSpeed", &ParticleEmitterMovementConfig::endRotSpeed);
        classInfo->addField("endRotSpeedVar", &ParticleEmitterMovementConfig::endRotSpeedVar);
    }
}

ParticleEmitterMovementConfig::ParticleEmitterMovementConfig() :
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

void ParticleEmitterRenderConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ParticleEmitterRenderConfig>("ParticleEmitterRenderConfig")) {
        classInfo->addResourceField("texture", ResourceType::Image, &ParticleEmitterRenderConfig::texture);
    }
}

ParticleEmitterRenderConfig::ParticleEmitterRenderConfig() :
    texture("") {
}