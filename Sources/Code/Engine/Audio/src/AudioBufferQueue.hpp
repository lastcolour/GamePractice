#ifndef __AUDIO_BUFFER_QUEUE_HPP__
#define __AUDIO_BUFFER_QUEUE_HPP__

#include "LockFreeQueue.hpp"

class AudioBuffer {
public:

    AudioBuffer();
    AudioBuffer(AudioBuffer&& other);
    AudioBuffer& operator=(AudioBuffer&& other);
    ~AudioBuffer();

    void write(void* from, int bytesCount);
    int read(void* to, int bytesCount);
    bool isDone() const;
    int getReadSize() const;

private:

    AudioBuffer(const AudioBuffer&) = delete;
    AudioBuffer& operator=(const AudioBuffer&) = delete;

private:

    std::unique_ptr<char[]> data;
    int offset;
    int size;
};

class AudioBufferQueue {
public:

    AudioBufferQueue();
    ~AudioBufferQueue();

    void init(int numBuffers);

    AudioBuffer* peekRead();
    void tryPopRead();

    std::vector<AudioBuffer*> peekWrites();
    void submitWrites(std::vector<AudioBuffer*>& writes);

private:

    std::vector<AudioBuffer> buffers;
    LockFreeQueue readQueue;
    LockFreeQueue writeQueue;
};

#endif /* __AUDIO_BUFFER_QUEUE_HPP__ */