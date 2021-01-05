#ifndef __PARTICLES_EMITTER_CONFIG_HPP__
#define __PARTICLES_EMITTER_CONFIG_HPP__

#include "Render/Color.hpp"
#include "Render/RenderCommon.hpp"
#include "Math/Vector.hpp"

#include <string>

class ReflectContext;

enum class EmitterType {
    Sphere = 0,
    Box
};

enum class EmitterSpace {
    Local = 0,
    World
};

class ParticleEmitterEmissionConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ParticleEmitterEmissionConfig();

public:

    EmitterType emitterType;
    EmitterSpace emitterSpace;
    Vec2 emitterVal;
    float duration;
    float direction;
    float directionVar;
    float emissionRate;
    float lifetime;
    float lifetimeVar;
    float startDelay;
    bool autoStart;
    bool loop;
};

class ParticleEmitterColorConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ParticleEmitterColorConfig();

public:

    ColorB startCol;
    Vec4 startColVar;
    ColorB endCol;
    Vec4 endColVar;
    float fadeIn;
    float fadeOut;
};

class ParticleEmitterMovementConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ParticleEmitterMovementConfig();

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

class ParticleEmitterRenderConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ParticleEmitterRenderConfig();

public:

    std::string texture;
};

#endif /* __PARTICLES_EMITTER_CONFIG_HPP__ */