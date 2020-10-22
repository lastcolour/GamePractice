#ifndef __PARTICLE_SYSTEM_HPP__
#define __PARTICLE_SYSTEM_HPP__

#include "Core/SystemLogic.hpp"

class ParticleSystem : public SystemLogic {
public:

    ParticleSystem();
    virtual ~ParticleSystem();

    // SystemLogic
    bool init() override;
    void deinit() override;

private:

};

#endif /* __PARTICLE_SYSTEM_HPP__ */