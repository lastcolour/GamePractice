#include "Particles/ParticlesEmitterConfig.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Reflect/EnumInfo.hpp"

void ParticleEmitterEmissionConfig::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<EmitterType>("EmitterType")) {
        enumInfo->addValues<EmitterType>({
            {"Sphere", EmitterType::Sphere},
            {"Cone", EmitterType::Cone},
            {"Box", EmitterType::Box},
        });
    }
    if(auto classInfo = ctx.classInfo<ParticleEmitterEmissionConfig>("ParticleEmitterEmissionConfig")) {
        classInfo->addField("emitterType", &ParticleEmitterEmissionConfig::emitterType);
        classInfo->addField("emitterVal", &ParticleEmitterEmissionConfig::emitterVal);
        classInfo->addField("duration", &ParticleEmitterEmissionConfig::duration);
        classInfo->addField("emissionRate", &ParticleEmitterEmissionConfig::emissionRate);
        classInfo->addField("lifetime", &ParticleEmitterEmissionConfig::lifetime);
        classInfo->addField("startDelay", &ParticleEmitterEmissionConfig::startDelay);
        classInfo->addField("autoStart", &ParticleEmitterEmissionConfig::autoStart);
        classInfo->addField("loop", &ParticleEmitterEmissionConfig::loop);
    }
}

ParticleEmitterEmissionConfig::ParticleEmitterEmissionConfig() :
    emitterType(EmitterType::Sphere),
    emitterVal(1.f, 0.f),
    duration(4.f),
    emissionRate(64.0f),
    lifetime(1.f),
    startDelay(0.f),
    autoStart(true),
    loop(true) {
}

void ParticleEmitterColorConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ParticleEmitterColorConfig>("ParticleEmitterColorConfig")) {
        classInfo->addField("startCol", &ParticleEmitterColorConfig::startCol);
        classInfo->addField("endCol", &ParticleEmitterColorConfig::endCol);
    }
}

ParticleEmitterColorConfig::ParticleEmitterColorConfig() :
    startCol(255, 255, 255),
    endCol(255, 0, 0) {
}

void ParticleEmitterMovementConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ParticleEmitterMovementConfig>("ParticleEmitterMovementConfig")) {
        classInfo->addField("startScale", &ParticleEmitterMovementConfig::startScale);
        classInfo->addField("endScale", &ParticleEmitterMovementConfig::endScale);
        classInfo->addField("startSpeed", &ParticleEmitterMovementConfig::startSpeed);
        classInfo->addField("endSpeed", &ParticleEmitterMovementConfig::endSpeed);
        classInfo->addField("startRotation", &ParticleEmitterMovementConfig::startRotation);
        classInfo->addField("startRotationSpeed", &ParticleEmitterMovementConfig::startRotationSpeed);
        classInfo->addField("endRotationSpeed", &ParticleEmitterMovementConfig::endRotationSpeed);
    }
}

ParticleEmitterMovementConfig::ParticleEmitterMovementConfig() :
    startScale(1.f),
    endScale(1.5f),
    startSpeed(1.f),
    endSpeed(2.f),
    startRotation(0.f),
    startRotationSpeed(30.f),
    endRotationSpeed(0.f) {
}

void ParticleEmitterRenderConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ParticleEmitterRenderConfig>("ParticleEmitterRenderConfig")) {
        classInfo->addResourceField("texture", ResourceType::Image, &ParticleEmitterRenderConfig::texture);
    }
}

ParticleEmitterRenderConfig::ParticleEmitterRenderConfig() :
    texture("") {
}