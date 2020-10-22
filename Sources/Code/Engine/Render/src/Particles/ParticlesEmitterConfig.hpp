#ifndef __PARTICLES_EMITTER_CONFIG_HPP__
#define __PARTICLES_EMITTER_CONFIG_HPP__

#include "Render/Color.hpp"
#include "Math/Vector.hpp"

#include <string>

class ReflectContext;

enum class EmitterType {
    Sphere = 0,
    Cone,
    Box
};

class ParticleEmitterEmissionConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ParticleEmitterEmissionConfig();

public:

    EmitterType emitterType;
    Vec2 emitterVal;
    float duration;
    float emissionRate;
    float lifetime;
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
    ColorB endCol;
};

class ParticleEmitterMovementConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ParticleEmitterMovementConfig();

public:

    Vec2 startScale;
    Vec2 endScale;
    float startSpeed;
    float endSpeed;
    float startRotation;
    float startRotationSpeed;
    float endRotationSpeed;
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