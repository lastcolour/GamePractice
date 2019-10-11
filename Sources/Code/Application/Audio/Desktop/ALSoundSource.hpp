#ifndef __AL_SOUND_SOURCE_HPP__
#define __AL_SOUND_SOURCE_HPP__

#include "Audio/SoundSource.hpp"

#include <vector>

const int MAX_BUFFERS_PER_STREAM = 4;

class ALSoundSource : public SoundSource {
public:

    ALSoundSource(unsigned int newSourceId);
    virtual ~ALSoundSource();

    bool init();
    void update();

    // SoundSource
    void attachToController(SoundSourceController& newController) override;
    void stopStreaming() override;
    void pauseStreaming() override;
    void resumeStreaming() override;
    bool isStreaming() const override;
    void setGain(float newGain) override;
    void setLoop(bool loopFlag) override;
    bool isLooped() const override;

private:

    enum class EBufferFillRes {
        Normal = 0,
        EndOfStream
    };

    enum class ESourceState {
        Normal = 0,
        WaitEnd,
        Ended
    };

private:

    void queueALBuffers(unsigned int* bufferIds, int size);
    EBufferFillRes fillALBuffer(unsigned int bufferId);
    void startALSource();
    void resetALSourceParams();

private:

    unsigned int alBufferIds[MAX_BUFFERS_PER_STREAM];
    SoundSourceController* controller;
    unsigned int sourceId;
    ESourceState state;
    bool looping;
};

#endif /* __AL_SOUND_SOURCE_HPP__ */