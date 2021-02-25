#ifndef __PARTICLES_EMITTER_CONFIG_HPP__
#define __PARTICLES_EMITTER_CONFIG_HPP__

#include "Render/Color.hpp"
#include "Render/RenderCommon.hpp"
#include "Math/Vector.hpp"
#include "Core/Core.hpp"

#include <string>
#include <vector>

class ReflectContext;

enum class EmitterType {
    Sphere = 0,
    Box
};

enum class EmitterSpace {
    Local = 0,
    World
};

class ParticlesEmitterEmissionConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ParticlesEmitterEmissionConfig();

public:

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

class ParticlesEmitterMovementConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ParticlesEmitterMovementConfig();

public:

    Vec2 startScale;
    Vec2 startScaleVar;
    Vec2 endScale;
    Vec2 endScaleVar;
    float startSpeed;
    float startSpeedVar;
    float endSpeed;
    float endSpeedVar;
    float startRotation;
    float startRotationVar;
    float startRotSpeed;
    float startRotSpeedVar;
    float endRotSpeed;
    float endRotSpeedVar;
};

class ParticlesEmitterRenderConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ParticlesEmitterRenderConfig();

public:

    std::string texture;
};

enum class GravityType {
    Simple = 0,
    Circle,
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
    OnStart = 0,
    OnParticleSpawn,
    OnParticleDeath,
    OnParticleLive,
    OnEnd
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