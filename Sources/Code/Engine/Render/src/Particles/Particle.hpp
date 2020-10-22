#ifndef __PARTICLE_HPP__
#define __PARTICLE_HPP__

#include "Math/Matrix.hpp"
#include "Render/Color.hpp"

struct Particle {
public:

    Particle();

public:

    Vec2 pt;
    Vec2 startSpeed;
    Vec2 endSpeed;
    Vec2 startScale;
    Vec2 endScale;
    float rot;
    float startRotSpeed;
    float endRotSpeed;
    float lifetime;
    float totalLifetime;
    ColorF startCol;
    ColorF endCol;
};

struct ParticleInstanceData {
public:

    ParticleInstanceData();

public:

    ColorF col;
    Math::Matrix4 mat;
};

#endif /* __PARTICLE_HPP__ */