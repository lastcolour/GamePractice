#ifndef __SOUND_SOURCE_HPP__
#define __SOUND_SOURCE_HPP__

#include <vector>

const int MAX_BUFFERS_PER_STREAM = 4;

class OggDataStream;

class SoundSource {
public:

    SoundSource(unsigned int newSourceId);
    virtual ~SoundSource();

    bool init();
    void reset();

    void startStreaming(OggDataStream& newDataStream);
    void stopStreaming();
    void updateStreaming();
    void pauseStreaming();
    void resumeStreaming();
    bool isStreaming() const;

private:

    enum class EBufferFillRes {
        Normal = 0,
        EndOfStream
    };

private:

    void queueALBuffers(unsigned int* bufferIds, int size);
    EBufferFillRes fillALBuffer(unsigned int bufferId);
    void startALSource();

private:

    unsigned int alBufferIds[MAX_BUFFERS_PER_STREAM];
    OggDataStream* dataStream;
    unsigned int sourceId;
    unsigned int samplesOffset;
    bool looping;
    bool ended;
};

#endif /* __SOUND_SOURCE_HPP__ */