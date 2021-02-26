#include "Particles/Particle.hpp"

Particle::Particle() :
    pt(0),
    speed(0.f),
    startScale(1.f),
    endScale(1.f),
    acc(0.f),
    rot(0.f),
    rotSpeed(0.f),
    lifetime(0.f),
    totalLifetime(0.f),
    startCol(1.f),
    endCol(1.f),
    id(-1) {
}

ParticleInstanceData::ParticleInstanceData() :
    col(1.f),
    mat() {
}