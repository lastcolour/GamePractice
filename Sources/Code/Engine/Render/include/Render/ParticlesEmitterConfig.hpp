#ifndef __PARTICLES_EMITTER_CONFIG_HPP__
#define __PARTICLES_EMITTER_CONFIG_HPP__

#include "Render/RenderCommon.hpp"

enum class EmitterType {
    Sphere = 0,
    Box
};

enum class EmitterSpace {
    Local = 0,
    World
};

enum class EmitterSimulationTime {
    Render = 0,
    Game,
    UI
};

class ParticlesEmitterEmissionConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ParticlesEmitterEmissionConfig();

public:

    EmitterSimulationTime emitterSimTime;
    EmitterType emitterType;
    EmitterSpace emitterSpace;
    Vec2 emitterVal;
    float duration;
    float thickness;
    float direction;
    float directionVar;
    float emissionRate;
    float lifetime;
    float lifetimeVar;
    float startDelay;
    bool autoStart;
    bool loop;
};

class ParticlesEmitterColorConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ParticlesEmitterColorConfig();

public:

    ColorB startCol;
    Vec4 startColVar;
    ColorB endCol;
    Vec4 endColVar;
    float fadeIn;
    float fadeOut;
};

class ParticlesEmitterSizeConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ParticlesEmitterSizeConfig();

public:

    Vec2 startScale;
    Vec2 startScaleVar;
    Vec2 endScale;
    Vec2 endScaleVar;
};

class ParticlesEmitterMovementConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ParticlesEmitterMovementConfig();

public:

    float speed;
    float speedVar;
    float initRotation;
    float initRotationVar;
    float rotationSpeed;
    float rotationSpeedVar;
    float rotationAcc;
};

class ParticlesEmitterRenderConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ParticlesEmitterRenderConfig();

public:

    TextureInfo textureInfo;
    EBlendMode blendMode;
};

enum class GravityType {
    Simple = 0,
    Vortex
};

class GravityField {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GravityField();

public:

    GravityType type;
    Vec2 offset;
    float dir;
    float value;
    float radius;
};

class ParticlesEmitterGravityConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    std::vector<GravityField> fields;
};

enum class SubEmitterTriggerEvent {
    None = 0,
    OnStart = 1 << 0,
    OnParticleSpawn = 1 << 1,
    OnParticleDeath = 1 << 2,
    OnParticleLive = 1 << 3,
    OnEnd = 1 << 4
};

class SubEmitter {
public:

    static void Reflect(ReflectContext& ctx);

public:

    SubEmitter();
    SubEmitter(SubEmitterTriggerEvent triggerEvent, EntityId entityId);

public:

    EntityId entId;
    SubEmitterTriggerEvent event;
};

class ParticlesSubEmittersConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ParticlesSubEmittersConfig();

public:

    std::vector<SubEmitter> subEmitters;
};

#endif /* __PARTICLES_EMITTER_CONFIG_HPP__ */