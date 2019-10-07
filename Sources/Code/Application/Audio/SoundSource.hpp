#ifndef __SOUND_SOURCE_HPP__
#define __SOUND_SOURCE_HPP__

#include <vector>

const int MAX_BUFFERS_PER_STREAM = 4;

class OggDataStream;

class SoundSource {
public:

    SoundSource(int newSourceId);
    virtual ~SoundSource();

    void startStreaming(OggDataStream* newDataStream);
    void stopStreaming();
    void updateStreaming();
    void pauseStreaming();
    void resumeStreaming();
    bool isFinishStreaming() const;
    bool isStreaming() const;

    void reset();

private:

    void queueBuffers(const std::vector<unsigned int>& bufferQueue);
    void startPlay();

private:

    unsigned int buffers[MAX_BUFFERS_PER_STREAM];
    OggDataStream* dataStream;
    int sourceId;
    bool looping;
};

#endif /* __SOUND_SOURCE_HPP__ */