#ifndef __PARTICLES_SOUND_EVENT_HPP__
#define __PARTICLES_SOUND_EVENT_HPP__

#include "Entity/EntityLogic.hpp"
#include "Audio/SoundEvent.hpp"
#include "Render/ETParticlesSystem.hpp"

class ParticlesSoundEvent : public EntityLogic,
    public ETNode<ETParticlesSystemEvent> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ParticlesSoundEvent();
    virtual ~ParticlesSoundEvent();

    // EntityLogic
    void init() override;
    void deinit() override;

    void ET_onEmitEvent(EParticlesEmitEventType emitFlags) override;

private:

    EParticlesEmitEventType emitType;
    SoundEvent soundEvent;
};

#endif /* __PARTICLES_SOUND_EVENT_HPP__ */