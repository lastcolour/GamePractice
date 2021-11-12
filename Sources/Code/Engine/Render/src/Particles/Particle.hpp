#ifndef __PARTICLE_HPP__
#define __PARTICLE_HPP__

#include "Math/Matrix.hpp"
#include "Math/Color.hpp"

struct Particle {
public:

    Particle();

public:

    Vec2 pt;
    Vec2 speed;
    Vec2 startScale;
    Vec2 endScale;
    Vec2 acc;
    float rot;
    float rotSpeed;
    float lifetime;
    float totalLifetime;
    ColorF startCol;
    ColorF endCol;
    int id;
};

struct ParticleInstanceData {
public:

    ParticleInstanceData();

public:

    ColorF col;
    Math::Matrix3x2 mat;
};

#endif /* __PARTICLE_HPP__ */