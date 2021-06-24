#ifndef __PARTICLES_UPDATE_FRAME_INFO_HPP__
#define __PARTICLES_UPDATE_FRAME_INFO_HPP__

class ParticlesUpdateFrameInfo {
public:

    ParticlesUpdateFrameInfo() :
        systemStarted(0),
        systemStopped(0),
        particlesSpawned(0),
        particlesRemoved(0) {}

    void reset() {
        systemStarted = 0;
        systemStopped = 0;
        particlesSpawned = 0;
        particlesRemoved = 0;
    }

public:

    int systemStarted;
    int systemStopped;
    int particlesSpawned;
    int particlesRemoved;
};

void DispatchParticlesSystemEvents(EntityId entId, ParticlesUpdateFrameInfo& updateInfo);

#endif /* __PARTICLES_UPDATE_FRAME_INFO_HPP__ */