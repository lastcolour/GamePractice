#ifndef __SOUND_SOURCE_HPP__
#define __SOUND_SOURCE_HPP__

#include <vector>

struct SoundSourceController;

const int MAX_BUFFERS_PER_STREAM = 4;

class SoundSource {
public:

    SoundSource(unsigned int newSourceId);
    virtual ~SoundSource();

    bool init();
    void update();

    void attachToController(SoundSourceController& newController);

    void stopStreaming();
    void pauseStreaming();
    void resumeStreaming();
    bool isStreaming() const;

    void setGain(float newGain);
    void setLoop(bool loopFlag);

    bool isLooped() const;

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

#endif /* __SOUND_SOURCE_HPP__ */